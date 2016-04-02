#include "bcpu.hpp"

// TODO: not availible on windows
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "io/string.hpp"
#include "io/input.hpp"
#include "io/file.hpp"

#define STATUS_BYTE 8

struct Params {
    std::vector<String> input;
    String output;
};

/*
struct Chunk {
    char sig[16];
    uint32_t addr;
    uint32_t size;
    char *data;
};

struct Cart {
    char sig[16];
    uint32_t hsize;
    uint32_t nchunks;
    Chunk *data;
};*/

struct OpEncode {
    char mnemonic[5];
    uint8_t op[6];
    int oplen;
};

struct Value {
    Bostek::Cpu::Type type;
    uint32_t val;
    bool symbol;

    Value() {}
    Value(Bostek::Cpu::Type t, uint32_t v, bool sym=false) : type(t), val(v), symbol(sym) {}
    Value(const Value &o) : type(o.type), val(o.val) {}

    // if no type is specified, then it is a register
    bool is_register() {
        return type == Bostek::Cpu::TYPE_NONE;
    }
};

int line = 0;
std::map<String, Value> symbols;

const char *arith_binary[] = {
    "ADD",
    "ADC",
    "SUB",
    "SBC",
    "CMP",
    "AND",
    "IOR",
    "XOR",
    "MUL",
    "DIV",
    "MOD",
    "POW",
    "MIN",
    "MAX",
    NULL,
};

const char *arith_unary[] = {
    "INC",
    "DEC",
    "TST",
    "COM",
    "NEG",
    "ABS",
    "SXT",
    "ZXT",
    "SHL",
    "SHR",
    "ROL",
    "ROR",
    NULL,
};

const char *ctrl_nulary[] = {
    "NOP",
    "HLT",
    "WFI",
    "RET",
    "RFI",
    "IRQ",
    "NMI",
    NULL,
};

const char *ctrl_unary[] = {
    "ANS",
    "ORS",
    "XRS",
    NULL,
};

void error(const char *msg) {
    printf("line %d: %s\n", line, msg);
    exit(-1);
}

int read_identifier(Input *in, char *dst, int max) {
    int i = 0;
    char c = in->peek();

    dst[max-1] = '\0';
    while(!in->eof() && isalpha(c)) {
        in->get();
        if(i < max-1) dst[i] = c;
        i++;
        c = in->peek();
    }

    if(i < max) dst[i] = '\0';
    return i;
}

Bostek::Cpu::Type char_to_type(char c) {
    switch(c) {
        case 'B':
            return Bostek::Cpu::TYPE_BYTE;
        case 'W':
            return Bostek::Cpu::TYPE_WORD;
        case 'L':
            return Bostek::Cpu::TYPE_LONG;
        case 'F':
            return Bostek::Cpu::TYPE_FLOAT;
        default:
            error("invalid size character.");
    }
}

void write_constant(uint8_t *mem, uint32_t *pc, Bostek::Cpu::Type type, uint32_t val) {
    mem[(*pc)++] = val & 0xFF;

    if(type >= Bostek::Cpu::TYPE_WORD) {
        mem[(*pc)++] = (val >> 8) & 0xFF;
    }

    if(type >= Bostek::Cpu::TYPE_LONG) {
        mem[(*pc)++] = (val >> 16) & 0xFF;
        mem[(*pc)++] = (val >> 24) & 0xFF;
    }
}

void read_mnemonic(Input *in, OpEncode *enc) {
    read_identifier(in, enc->mnemonic, 5);
}

int index_in_list(OpEncode *enc, const char **lst) {
    int i = 0;
    while(lst[i] != NULL) {
        if(!strncmp(enc->mnemonic, lst[i], 3)) return i;
        i++;
    }
    return -1;
}

bool next_is_value(Input *in) {
    char c = in->peek();
    while(!in->eof() && (isspace(c) && c != '\n')) {
        in->get();
        c = in->peek();
    }

    if(in->eof()) return false;
    if(c == ';') return false;
    if(c == '\n') return false;
    return c == '$' || c == '-' || isalnum(c);
}

Value read_value(Input *in) {
    // ignore '#' for decimal numbers
    uint64_t val = 0;
    bool negative = false;

    while(in->peek() == ' ' && !in->eof()) in->get();

    char c = in->peek();

    if(c == '-') {
        negative = true;
        in->get();
        c = in->peek();
    }

    if(c == '#') {
        in->get();
        c = in->peek();
    }

    if(isalpha(c)) { // is symbol
        char sym[9];
        read_identifier(in, sym, 9);

        if(!symbols.count(sym)) {
            printf("unknown symbol %s\n", sym);
            exit(-1);
        }

        return symbols[sym];
    } else if(isdigit(c)) { // is decimal number
        while(!in->eof() && !isspace(c) && c != ';') {
            if(!isdigit(c)) {
                error("invalid decimal value");
            }

            val *= 10;
            val += (c - '0');

            in->get();
            c = in->peek();
        }
    } else if(c == '$') { // is hex number
        in->get();
        c = in->peek();
        while(!in->eof() && !isspace(c) && c != '+' && c != ';') {
            if(!isxdigit(c)) {
                error("invalid hex value");
                exit(-1);
            }

            val <<= 4;
            val += (c - '0');

            in->get();
            c = in->peek();
        }
    } else if(c == '\'') { // is ascii character
    }

    uint64_t absv = val;

    if(negative) {
        val = (~val) + 1;
    }

    if(absv < 0xFF || (absv <= 0xFF && !negative)) {
        return Value(Bostek::Cpu::TYPE_BYTE, val);
    } else if(absv < 0xFFFF || (absv <= 0xFFFF && !negative)) {
        return Value(Bostek::Cpu::TYPE_WORD, val);
    } else if(absv < 0xFFFFFFF || (absv <= 0xFFFFFFF && !negative)) {
        return Value(Bostek::Cpu::TYPE_LONG, val);
    } else {
        error("value too large");
        exit(-1);
    }
}

void process_op(uint8_t *mem, Input *in, uint32_t *pc) {
    OpEncode enc;
    read_mnemonic(in, &enc);

    int i;
    if(enc.mnemonic[0] == 'J') {
        Value target = read_value(in);
        if(target.type == Bostek::Cpu::TYPE_NONE) {
            error("jump target must be label or constant; not register");
        }

        if(!strncmp(enc.mnemonic, "JMP", 3)) {
            mem[(*pc)++] = 0x64;
            uint16_t rel_addr = target.val - ((*pc) + 2);
            mem[(*pc)++] = rel_addr & 0xFF;
            mem[(*pc)++] = (rel_addr >> 8) & 0xFF;
            //TODO: long jump
        } else if(enc.mnemonic[2] == 'C' || enc.mnemonic[2] == 'S') {
            mem[*pc] = 0x70;
            switch(enc.mnemonic[1]) {
                case 'C':
                    break;
                case 'H':
                    mem[*pc] += 1;
                    break;
                case 'F':
                    mem[*pc] += 2;
                    break;
                case 'T':
                    mem[*pc] += 3;
                    break;
                case 'I':
                    mem[*pc] += 4;
                    break;
                case 'V':
                    mem[*pc] += 5;
                    break;
                case 'Z':
                    mem[*pc] += 6;
                    break;
                case 'S':
                    mem[*pc] += 7;
                    break;
                default:
                    printf("unknown mnemonic %s\n", enc.mnemonic);
                    exit(-1);
            }

            if(enc.mnemonic[2] == 'S') {
                mem[*pc] += 8;
            }
            (*pc)++;

            uint16_t rel_addr = target.val - ((*pc) + 2);
            mem[(*pc)++] = rel_addr & 0xFF;
            mem[(*pc)++] = (rel_addr >> 8) & 0xFF;
        } else {
            printf("unknown mnemonic %s\n", enc.mnemonic);
            exit(-1);
        }
    } else if (!strncmp(enc.mnemonic, "LOD", 3)) {
        Value dst = read_value(in);
        Value base = read_value(in);
        uint16_t rel_addr = base.val - ((*pc) + 4);
        Bostek::Cpu::Type type = char_to_type(enc.mnemonic[3]);

        if(!dst.is_register()) {
            error("load destination must be register");
        }

        if(base.is_register()) {
            error("load base must be constant");
        }

        // relative load
        if(in->peek() == '+') {
            in->get();
            Value offset = read_value(in);
            if(!offset.is_register()) {
                error("load offset must be register");
            }
            mem[(*pc)++] = 0x20 + (uint8_t) type;
            mem[(*pc)++] = ((offset.val << 4) & 0xF0) | (dst.val & 0x0F);

            write_constant(mem, pc, Bostek::Cpu::TYPE_WORD, rel_addr);
        } else {
            mem[(*pc)++] = 0x10 + (uint8_t) type;
            mem[(*pc)++] = dst.val & 0x0F;
            write_constant(mem, pc, Bostek::Cpu::TYPE_WORD, rel_addr);
        }
    } else if (!strncmp(enc.mnemonic, "STO", 3)) {
        Value base = read_value(in);
        uint16_t rel_addr = base.val - ((*pc) + 4);
        Bostek::Cpu::Type type = char_to_type(enc.mnemonic[3]);


        if(base.is_register()) {
            error("store base must be constant");
        }

        // relative load
        if(in->peek() == '+') {
            in->get();
            Value offset = read_value(in);
            Value src = read_value(in);
            if(!offset.is_register()) {
                error("store offset must be register");
            }
            if(!src.is_register()) {
                error("store source must be register");
            }
            mem[(*pc)++] = 0x28 + (uint8_t) type;
            mem[(*pc)++] = ((offset.val << 4) & 0xF0) | (src.val & 0x0F);
            write_constant(mem, pc, Bostek::Cpu::TYPE_WORD, rel_addr);
        } else {
            Value src = read_value(in);
            if(!src.is_register()) {
                error("store source must be register");
            }
            mem[(*pc)++] = 0x18 + (uint8_t) type;
            mem[(*pc)++] = src.val & 0x0F;
            write_constant(mem, pc, Bostek::Cpu::TYPE_WORD, rel_addr);
        }
    } else if (!strncmp(enc.mnemonic, "MOV", 3)) {
        Value v1 = read_value(in);
        Value v2 = read_value(in);
        Bostek::Cpu::Type type = char_to_type(enc.mnemonic[3]);

        if(!v1.is_register()) {
            error("first op to MOV must be register");
        }

        if(v1.val == STATUS_BYTE && v2.is_register()) {
            if(v2.val == STATUS_BYTE) {
                error("cannot move SB to SB");
            }
            mem[(*pc)++] = 0x38;
            mem[(*pc)++] = 0xF0 & (v2.val << 4);
        } else if(v1.val == STATUS_BYTE && !v2.is_register()) {
            mem[(*pc)++] = 0x3C;
            mem[(*pc)++] = 0x00;
            mem[(*pc)++] = v2.val & 0xFF;
        } else if(v2.is_register() && v2.val == STATUS_BYTE) {
            mem[(*pc)++] = 0x3A;
            mem[(*pc)++] = 0x0F & v1.val;
        } else if(v2.is_register()) {
            mem[(*pc)++] = 0x30 + (uint8_t) type;
            mem[(*pc)++] = (0xF0 & (v2.val << 4)) | (0x0F & v1.val);
        } else {
            mem[(*pc)++] = 0x34 + (uint8_t) type;
            mem[(*pc)++] = 0x0F & v1.val;
            write_constant(mem, pc, type, v1.val);
        }
    } else if (!strncmp(enc.mnemonic, "PSH", 3)) {
        uint8_t opcode = 0x4C;

        Value v1 = read_value(in);

        if(v1.is_register() && v1.val == STATUS_BYTE) {
            mem[(*pc)++] = 0x4D;
            mem[(*pc)++] = 0xF0 & ((uint8_t) char_to_type(enc.mnemonic[3]) << 4);
        } else if(v1.is_register()) {
            mem[(*pc)++] = 0x4C;
            mem[(*pc)++] = (0xF0 & ((uint8_t) char_to_type(enc.mnemonic[3]) << 4)) | (v1.val & 0x0F);
        } else {
            mem[(*pc)++] = 0x4E;
            Bostek::Cpu::Type type = char_to_type(enc.mnemonic[3]);
            mem[(*pc)++] = 0xF0 & (((uint8_t) type) << 4);
            write_constant(mem, pc, type, v1.val);
        }
    } else if (!strncmp(enc.mnemonic, "POP", 3)) {
        uint8_t opcode = 0x48;

        if(!next_is_value(in)) {
            mem[(*pc)++] = 0x4A;
            mem[(*pc)++] = 0xF0 & ((uint8_t) char_to_type(enc.mnemonic[3]) << 4);
        }

        Value v1 = read_value(in);

        if(v1.is_register() && v1.val == STATUS_BYTE) {
            mem[(*pc)++] = 0x49;
            mem[(*pc)++] = 0xF0 & ((uint8_t) char_to_type(enc.mnemonic[3]) << 4);
        } else if(v1.is_register()) {
            mem[(*pc)++] = 0x48;
            mem[(*pc)++] = (0xF0 & ((uint8_t) char_to_type(enc.mnemonic[3]) << 4)) | (v1.val & 0x0F);
        }
    } else if (!strncmp(enc.mnemonic, "SWP", 3)) {
        uint8_t opcode = 0x40 + (uint8_t) char_to_type(enc.mnemonic[3]);
        Value v1 = read_value(in);
        Value v2 = read_value(in);

        if(!v1.is_register() || !v2.is_register()) {
            error("both ops to SWP must be registers");
        }

        mem[(*pc)++] = opcode;
        mem[(*pc)++] = ((v2.val << 4) & 0xF0) | (v1.val & 0x0F);
    } else if((i = index_in_list(&enc, arith_binary)) >= 0) {
        uint8_t opcode = 0x80 + 8 * i;

        Bostek::Cpu::Type type;
        switch(enc.mnemonic[3]) {
            case 'B':
                type = Bostek::Cpu::TYPE_BYTE;
                break;
            case 'W':
                type = Bostek::Cpu::TYPE_WORD;
                opcode += 1;
                break;
            case 'L':
                type = Bostek::Cpu::TYPE_LONG;
                opcode += 2;
                break;
            case 'F':
                type = Bostek::Cpu::TYPE_FLOAT;
                opcode += 3;
                break;
            default:
                printf("invalid binary mnemonic '%s'. expected size\n", enc.mnemonic);
                exit(-1);
        }

        Value v1 = read_value(in);
        Value v2 = read_value(in);

        if(!v1.is_register()) {
            error("first value of binary arithmetic must be register");
            exit(-1);
        }

        if(v2.is_register()) {
            mem[(*pc)++] = opcode;
            mem[(*pc)++] = ((v2.val << 4) & 0xF0) | (v1.val & 0x0F);
        } else { // RK
            mem[(*pc)++] = opcode + 8;
            mem[(*pc)++] = v1.val & 0x0F;
            if(v2.type > type) {
                error("constant too large in binary arithmetic");
            }
            write_constant(mem, pc, type, v2.val);
        }
    } else if((i = index_in_list(&enc, arith_unary)) >= 0) {
        uint8_t opcode = 0xF0 + i;
        Value v1 = read_value(in);

        Bostek::Cpu::Type type;
        switch(enc.mnemonic[4]) {
            case 'B':
                type = Bostek::Cpu::TYPE_BYTE;
                break;
            case 'W':
                type = Bostek::Cpu::TYPE_WORD;
                break;
            case 'L':
                type = Bostek::Cpu::TYPE_LONG;
                break;
            case 'F':
                type = Bostek::Cpu::TYPE_FLOAT;
                break;
            default:
                error("invalid unary arithmetic mnemonic. expected size");
        }

        if(!v1.is_register()) {
            error("operand of unary expected to be register");
        }

        mem[(*pc)++] = opcode;
        mem[(*pc)++] = ((((uint8_t) type) << 4) & 0xF0) | (v1.val & 0x0F);
    } else if((i = index_in_list(&enc, ctrl_nulary)) >= 0) {
        mem[(*pc)++] = i;
    } else if((i = index_in_list(&enc, ctrl_unary)) >= 0) {
        Value v1 = read_value(in);
        if(v1.is_register()) {
            mem[(*pc)++] = 0x08 + i;
            mem[(*pc)++] = v1.val & 0xFF;
        } else {
            mem[(*pc)++] = 0x0C + i;
            mem[(*pc)++] = v1.val & 0xFF;
        }
    } else if(!strncmp(enc.mnemonic, "CPU", 3)) {
        mem[(*pc)++] = 0x07;
        Value v1 = read_value(in);
        if(!v1.is_register()) {
            error("expected register for CPUB");
        }
        mem[(*pc)++] = v1.val & 0xFF;
    } else {
        printf("unknown mnemonic: %s\n", enc.mnemonic);
        exit(-1);
    }
}

void process_special(uint8_t *mem, Input *in, uint32_t *pc) {
    in->get();
    char id[9];
    read_identifier(in, id, 9);
    if(!strncmp(id, "ORG", 4)) {
        Value v = read_value(in);
        *pc = v.val;
    }
}

void parse_file(uint8_t *mem, Input *in) {
    uint32_t pc = 0x1000;
    char c = in->peek();
    while(!in->eof()) {
        if(c == '.') {
            process_special(mem, in, &pc);
        } else if(isspace(c)) {
            if(c == '\n') line++;
            in->get();
            c = in->peek();
        } else if(c == ';') {
            while(!in->eof() && c != '\n') {
                in->get();
                c = in->peek();
            }
        } else {
            process_op(mem, in, &pc);
        }
        in->get();
        c = in->peek();
    }
}

Params parse_params(int argc, char **argv) {
    Params params;
    while(optind < argc) {
        char c = getopt(argc, argv, "-o:");
        switch(c) {
            case 'o':
                params.output = optarg;
                break;
            case '?':
                std::cout << "missing argument for -" << (char) optopt << std::endl;
                exit(-1);
                break;
            default:
                params.input.push_back(optarg);
                break;
        }
    }
    if(params.output.empty()) {
        error("expect output file");
    }
    return params;
}



int main(int argc, char **argv) {
    Params p = parse_params(argc, argv);

    // push back register symbols
    symbols["A"] = Value(Bostek::Cpu::TYPE_NONE, 0, true);
    symbols["B"] = Value(Bostek::Cpu::TYPE_NONE, 1, true);
    symbols["C"] = Value(Bostek::Cpu::TYPE_NONE, 2, true);
    symbols["D"] = Value(Bostek::Cpu::TYPE_NONE, 3, true);
    symbols["AH"] = Value(Bostek::Cpu::TYPE_NONE, 4, true);
    symbols["BH"] = Value(Bostek::Cpu::TYPE_NONE, 5, true);
    symbols["CH"] = Value(Bostek::Cpu::TYPE_NONE, 6, true);
    symbols["DH"] = Value(Bostek::Cpu::TYPE_NONE, 7, true);
    symbols["SB"] = Value(Bostek::Cpu::TYPE_NONE, 8, true);

    uint8_t *mem = (uint8_t*) malloc(0x10000);
    for(int i = 0; i < p.input.size(); i++) {
        File f(p.input[i]);
        parse_file(mem, &f);
    }

    FILE *output = fopen(p.output.c_str(), "w");
    fwrite(mem, 1, 0x10000, output);
    fclose(output);

    free(mem);

    return 0;
}
