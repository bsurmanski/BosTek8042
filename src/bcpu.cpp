#include "bcpu.hpp"

#include <string.h>

#include <stdio.h>

using namespace Bostek;
using namespace Bostek::Cpu;

uint32_t BCpu::sign_mask(Type ty) {
    switch(ty) {
        case TYPE_NONE:
        case TYPE_BYTE: return 0x80;
        case TYPE_WORD: return 0x8000;
        case TYPE_FLOAT:
        case TYPE_LONG: return 0x80000000;
    }
    return 0;
}

uint32_t BCpu::type_mask(Type ty) {
    switch(ty) {
        case TYPE_NONE:
        case TYPE_BYTE: return 0xFF;
        case TYPE_WORD: return 0xFFFF;
        case TYPE_FLOAT:
        case TYPE_LONG: return 0xFFFFFFFF;
    }
    return 0;
}

bool BCpu::is_negative(Type ty, uint32_t val) {
    return sign_mask(ty) & val;
}

uint32_t BCpu::zxt_value(Type ty, uint32_t val) {
    if(ty == TYPE_BYTE) return val;
    return val & type_mask(ty);
}

uint32_t BCpu::sxt_value(Type ty, uint32_t val) {
    if(is_negative(ty, val)) {
        return (type_mask(TYPE_LONG) ^ type_mask(ty)) | val;
    }
    return zxt_value(ty, val);
}

uint32_t BCpu::neg_value(Type ty, uint32_t val) {
    return ((~val)+1) & type_mask(ty);
}

uint32_t BCpu::abs_value(Type ty, uint32_t val) {
    if(sign_mask(ty) & val) return neg_value(ty, val);
    return val;
}

uint64_t BCpu::pow_value(uint32_t v1, uint32_t v2) {
    uint64_t ret = 1;
    while(v2) {
        if(v2 & 0x01) ret *= v1;
        v2 >>= 1;
        v1 *= v1;
    }
    return ret;
}

Delta::Delta() : wb_type(TYPE_NONE) {
}

Delta::Delta(State s) : next(s), wb_type(TYPE_NONE) {
}

Delta::Delta(State s, Type ty, uint32_t addr, uint32_t v) : next(s), wb_type(ty), wb_addr(addr), wb_value(v) {
}

BCpu::BCpu() {
}

BCpu::BCpu(uint32_t pc, uint32_t sp) {
    state.pc = pc;
    state.sp = sp;
}

State::State() : pc(0), sp(0), sb(0) {
    memset(registers, 0, sizeof(registers));
    memset(fregisters, 0, sizeof(fregisters));
}

uint32_t State::read_register(uint8_t reg, uint8_t type) {
    float v;
    switch(type) {
        case 0: return readb_register(reg);
        case 1: return readw_register(reg);
        case 2: return readl_register(reg);
        case 3:
                v = readf_register(reg);
                return reinterpret_cast<uint32_t&>(v);
    }
    return 0;
}

void State::write_register(uint8_t reg, uint8_t type, uint32_t val) {
    float v;
    switch(type) {
        case 0: writeb_register(reg, val & 0xFF);
                break;
        case 1: writew_register(reg, val & 0xFFFF);
                break;
        case 2: writel_register(reg, val);
                break;
        case 3:
                writef_register(reg, reinterpret_cast<float&>(val));
                break;
    }
}

uint8_t State::readb_register(uint8_t reg) {
    if(reg < 4) {
        return registers[reg] & 0xFF;
    } else if(reg < 8) {
        return (registers[reg-4] & 0xFF00) >> 8;
    }
    return 0;
}

void State::writeb_register(uint8_t reg, uint8_t val) {
    if(reg < 4) {
        registers[reg] = (registers[reg] & 0xFFFFFF00) | val;
    } else if(reg < 8) {
        registers[reg-4] = (registers[reg-4] & 0xFFFF00FF) | (val << 8);
    }
}

uint16_t State::readw_register(uint8_t reg) {
    if(reg < 4) {
        return registers[reg] & 0xFFFF;
    } else if(reg < 8) {
        return (registers[reg-4] & 0xFFFF0000) >> 16;
    }
    return 0;
}

void State::writew_register(uint8_t reg, uint16_t val) {
    if(reg < 4) {
        registers[reg] = (registers[reg] & 0xFFFF0000) | val;
    } else if(reg < 8) {
        registers[reg-4] = (registers[reg-4] & 0x0000FFFF) | (val << 16);
    }
}

uint32_t State::readl_register(uint8_t reg) {
    if(reg < 4) {
        return registers[reg];
    }
    return 0;
}

void State::writel_register(uint8_t reg, uint32_t val) {
    if(reg < 4) {
        registers[reg] = val;
    }
}

float State::readf_register(uint8_t reg) {
    if(reg < 4) {
        return fregisters[reg];
    }
    return 0;
}

void State::writef_register(uint8_t reg, float val) {
    if(reg < 4) {
        fregisters[reg] = val;
    }
}

bool State::read_flag(Flag f) {
    return (bool) (sb & (0x01 << ((uint8_t) f)));
}

void State::write_flag(Flag f, bool b) {
    sb = (sb & ~(0x01 << (uint8_t) f)) | (b << (uint8_t) f);
}

bool State::flags_set(uint8_t flags) {
    return (~sb & flags) == 0;
}

bool State::flags_clear(uint8_t flags) {
    return (sb & flags) == 0;
}

Delta BCpu::decode_control(uint8_t op1) {
    State next(state);
    uint8_t op2;

    if(op1 <= NMI) { // NULARY
        switch(op1) {
            case NOP:
                next.pc++;
                break;
            case HLT:
                break;
            case WFI:
            case RET:
            case RFI:
            case IRQ:
            case NMI:
                next.pc++;
                break;
                //TODO
        }
    } else { // UNAK8 (ANSB, ORSB, XRSB)
        op2 = nbr->readb(next.pc+1);
        next.pc+=2;
        if(op1 <= XRSB_R) { // if register version
            op2 = state.readb_register(op2 & 0x0F);
        }

        switch(op1) {
            case ANSB_R:
            case ANSB_K:
                next.sb = state.sb & op2;
                break;
            case ORSB_R:
            case ORSB_K:
                next.sb = state.sb | op2;
                break;
            case XRSB_R:
            case XRSB_K:
                next.sb = state.sb ^ op2;
                break;
        }
    }

    return next;
}

Delta BCpu::decode_move(uint8_t op1) {
    State next(state);
    Type wb_type = TYPE_NONE; // used for PSH/POP
    uint32_t wb_addr = 0; // used for PSH/POP
    uint32_t wb_value = 0; // used for PSH/POP

    if(op1 <= LSTOF_RRK) { // load/store
        //TODO
    } else if(op1 <= MOVB_SK) { // move, load constant
        bool mov_to_sb = (op1 & 0x08) && !(op1 & 0x02);
        bool mov_from_sb = (op1 & 0x08) && (op1 & 0x02);
        bool use_constant = op1 & 0x04;
        uint8_t op2 = nbr->readb(state.pc+1);
        uint8_t type;
        if(mov_to_sb || mov_from_sb) {
            type = TYPE_BYTE;
        } else {
            type = op1 & 0x03;
        }
        uint32_t val;
        if(use_constant) { // move in constant
            switch(type) {
                case TYPE_BYTE:
                    val = nbr->readb(state.pc+2);
                    next.pc+=3;
                    break;
                case TYPE_WORD:
                    val = nbr->readw(state.pc+2);
                    next.pc+=4;
                    break;
                case TYPE_LONG:
                case TYPE_FLOAT:
                    val = nbr->readl(state.pc+2);
                    next.pc+=6;
                    break;
            }
        } else if(mov_from_sb) {
            val = state.sb;
            next.pc += 2;
        } else { // move from register
            val = state.read_register((op2 & 0xF0) >> 4, type);
            next.pc+=2;
        }

        if(mov_to_sb) {
            next.sb = val;
        } else {
            next.write_register(op2 & 0x0F, type, val);
        }
    } else if(op1 <= SWPF) { // swap
        uint8_t op2 = nbr->readb(state.pc+1);
        uint8_t reg1 = op2 & 0x0F;
        uint8_t reg2 = (op2 & 0xF0) >> 4;
        uint8_t type = op1 & 0x03;
        uint32_t v1 = state.read_register(reg1, type);
        uint32_t v2 = state.read_register(reg2, type);
        next.write_register(reg1, type, v2);
        next.write_register(reg2, type, v1);
        next.pc += 2;
    } else if(op1 <= PSHX_K) {
        uint8_t op2 = nbr->readb(state.pc+1);
        uint8_t type = (op2 & 0x30) >> 4;

        //PSH
        if(op1 & 0x04) {
            uint32_t v;

            // find value to write
            // update next pc
            switch(op1) {
                case PSHX_R:
                    v = state.read_register(op2 & 0x0F, type);
                    next.pc += 2;
                    break;
                case PSHX_S:
                    v = state.sb;
                    next.pc += 2;
                    break;
                case PSHX_K:
                    switch(type) {
                        case TYPE_BYTE:
                            v = nbr->readb(state.pc+2);
                            next.pc += 3;
                            break;
                        case TYPE_WORD:
                            v = nbr->readw(state.pc+2);
                            next.pc += 4;
                            break;
                        case TYPE_LONG:
                        case TYPE_FLOAT:
                            v = nbr->readw(state.pc+2);
                            next.pc += 6;
                            break;
                    }
                    break;
            }

            // write value
            // update sp
            switch(type) {
                case TYPE_BYTE:
                    next.sp-=1;
                    wb_type = TYPE_BYTE;
                    wb_addr = next.sp;
                    wb_value = v;
                    break;
                case TYPE_WORD:
                    next.sp-=2;
                    wb_type = TYPE_WORD;
                    wb_addr = next.sp;
                    wb_value = v;
                    break;
                case TYPE_LONG:
                case TYPE_FLOAT:
                    next.sp-=4;
                    wb_type = TYPE_LONG;
                    wb_addr = next.sp;
                    wb_value = v;
                    break;
            }
        } else { // else POP
            // read value
            // update sp
            uint32_t v;
            switch(type) {
                case TYPE_BYTE:
                    v = nbr->readb(next.sp);
                    next.sp+=1;
                    break;
                case TYPE_WORD:
                    v = nbr->readw(next.sp);
                    next.sp+=2;
                    break;
                case TYPE_LONG:
                case TYPE_FLOAT:
                    v = nbr->readl(next.sp);
                    next.sp+=4;
                    break;
            }

            switch(op1) {
                case POPX_R:
                    next.write_register(op2 & 0x0F, type, v);
                    break;
                case POPX_S:
                    next.sb = v & 0x000000FF;
                    break;
                case POPX_X:
                    break;
            }
            next.pc += 2;
        }
    }

    return Delta(next, wb_type, wb_addr, wb_value);
}

Delta BCpu::decode_jump(uint8_t op1) {
    State next(state);
    Type wb_type = TYPE_NONE; // used for JSR
    uint32_t wb_addr; // used for JSR
    uint32_t wb_value = 0; // used for JSR

    if(op1 < 0x70) { // jmp, jsr, rjmp, rjsr
        uint32_t target;
        bool is_long = op1 & 0x01;
        bool is_jsr = op1 & 0x02;
        bool is_relative = op1 & 0x04;
        if(is_long) { // long jump
            next.pc += 5;
            target = nbr->readl(state.pc+1);
        } else { // 16-bit jump
            next.pc += 3;
            target = nbr->readw(state.pc+1);
            if((target & 0x00008000) &&
               is_relative) target |= 0xFFFF0000; //sign extend (only if relative)
        }

        if(is_jsr) {
            wb_type = TYPE_LONG;
            next.sp -= 4;
            wb_addr = next.sp;
            wb_value = next.pc; // take into account constant offset
        }

        if(is_relative) {
            next.pc += target;
        } else { // absolute
            next.pc = target;
        }
    } else { // J*C, J*S
        next.pc += 3;
        uint32_t offset = nbr->readw(state.pc+1);
        if(offset & 0x00008000) offset |= 0xFFFF0000; //sign extend

        if(state.read_flag((Flag) (op1 & 0x07)) == (bool)(op1 & 0x08)){
            next.pc += offset;
        }
    }
    return Delta(next, wb_type, wb_addr, wb_value);
}

Delta BCpu::decode_arithmetic(uint8_t op1) {
    State next(state);
    uint8_t op2 = nbr->readb(state.pc+1);

    if(op1 < 0xF0) { // Binary Arithmetic
        bool imm = op1 & 0x04;
        uint8_t reg1 = op2 & 0x0F;
        Type type = (Type) (op1 & 0x03);
        uint32_t v1 = state.read_register(reg1, type);
        uint32_t v2;
        uint32_t wb = 0;
        bool write_enable = true;
        uint32_t smask = sign_mask(type);
        bool sign_parity;

        if(imm) { // uses immediate constant
            switch(type) {
                case TYPE_NONE:
                case TYPE_BYTE:
                    v2 = nbr->readb(state.pc+2);
                    next.pc+=3;
                    break;
                case TYPE_WORD:
                    v2 = nbr->readw(state.pc+2);
                    next.pc+=4;
                    break;
                case TYPE_LONG:
                    v2 = nbr->readl(state.pc+2);
                    next.pc+=6;
                    break;
                case TYPE_FLOAT:
                    //TODO:!!!
                    //f2 = nbr->readf(state.pc+2);
                    break;
            }
        } else {
            v2 = state.read_register((op2 & 0xF0) >> 4, type);
            next.pc += 2;
        }

        switch(op1 & 0xF8) { // switch on op type (add, adc, sub, etc)
            case ADD:
                wb = v1 + v2;
                next.write_flag(FLAG_C, (smask & v1 & v2) || (smask & v1 & ~wb) || (smask & v2 & ~wb));
                next.write_flag(FLAG_V, (smask & ~wb & v1 & v2) || (smask & wb & ~v1 & ~v2));
                break;
            case ADC:
                wb = v1 + v2 + state.read_flag(FLAG_C);
                next.write_flag(FLAG_C, (smask & v1 & v2) || (smask & v1 & ~wb) || (smask & v2 & ~wb));
                next.write_flag(FLAG_V, (smask & ~wb & v1 & v2) || (smask & wb & ~v1 & ~v2));
                break;
            case SUB:
                wb = v1 - v2;
                next.write_flag(FLAG_C, (smask & wb & ~v1 & ~v2) || (smask & ~wb & v1 & v2));
                next.write_flag(FLAG_V, (smask & ~wb & v1 & ~v2) || (smask & wb & ~v1 & v2));
                break;
            case SBC:
                wb = v1 - v2 - state.read_flag(FLAG_C);
                next.write_flag(FLAG_C, (smask & wb & ~v1 & ~v2) || (smask & ~wb & v1 & v2));
                next.write_flag(FLAG_V, (smask & ~wb & v1 & ~v2) || (smask & wb & ~v1 & v2));
                break;
            case CMP:
               wb = v1 - v2;
               next.write_flag(FLAG_C, (smask & wb & ~v1 & ~v2) || (smask & ~wb & v1 & v2));
               next.write_flag(FLAG_V, (smask & ~wb & v1 & ~v2) || (smask & wb & ~v1 & v2));
               write_enable = false;
               break;
            case AND:
                wb = v1 & v2;
                break;
            case OR:
                wb = v1 | v2;
                break;
            case XOR:
                wb = v1 ^ v2;
                break;
            case MIN:
                if(v1 < v2) wb = v1;
                else wb = v2;
                break;
            case MAX:
                if(v1 > v2) wb = v1;
                else wb = v2;
                break;

            case MUL:
                wb = abs_value(type, v1) * abs_value(type, v2);
                if(abs_value(type, v1) != 0 &&
                   (wb & type_mask(type)) / abs_value(type, v1) != abs_value(type, v2)) {
                    next.write_flag(FLAG_C, true);
                    next.write_flag(FLAG_V, true);
                } else {
                    next.write_flag(FLAG_C, false);
                    next.write_flag(FLAG_V, (smask & ~wb & v1 & v2) || (smask & wb & ~v1 & ~v2));
                }

                // if only one is negative
                if(smask & (v1 ^ v2)) wb = neg_value(type, wb);
                break;

            case POW:
                if(v2 & smask) {
                    wb = 0;
                    next.write_flag(FLAG_C, false);
                } else {
                    wb = pow_value(abs_value(type, v1), abs_value(type, v2));

                    // catch overflow if value WAY overflows
                    if ((wb > type_mask(type)) ||
                        (abs_value(type, v1) >= 2 && v2 >= 32)) {
                        next.write_flag(FLAG_C, true);
                        next.write_flag(FLAG_V, true);
                    } else {
                        next.write_flag(FLAG_C, false);
                        next.write_flag(FLAG_V, (smask & ~wb & v1 & v2) || (smask & wb & ~v1 & ~v2));
                    }
                    if(v1 & smask && v2 & 0x01)  // if v1 is negative, and v2 is odd
                        wb = neg_value(type, wb);
                }
                break;

            case DIV:
                // division by zero is an error
                if(!v2 & type_mask(type)) {
                    wb = type_mask(type); // if v2 is zero, then saturate
                    next.write_flag(FLAG_T, true); // trap on div 0
                } else {
                    wb = abs_value(type, v1) / abs_value(type, v2);
                }

                next.write_flag(FLAG_C, !v2); // only way to carry is v2 == 0
                next.write_flag(FLAG_V, !v2);

                // if only one is negative
                if(smask & (v1 ^ v2)) wb = neg_value(type, wb);
                break;

            case MOD:
                // mod by zero is an error
                if(!v2 & type_mask(type)) {
                    wb = type_mask(type); // if v2 is zero, then saturate
                    next.write_flag(FLAG_T, true); // trap on div 0
                } else {
                    wb = abs_value(type, v1) % abs_value(type, v2);
                }
                next.write_flag(FLAG_C, !v2); // only way to overflow is v2 == 0
                if(sign_parity) wb = neg_value(type, wb);
                break;
        }

        if(write_enable) next.write_register(reg1, type, wb);
        next.write_flag(FLAG_S, smask & wb);
        next.write_flag(FLAG_Z, !(wb & type_mask(type)));
    } else if(op1 <= RORX) { // unary
        next.pc += 2;
        uint8_t reg1 = op2 & 0x0F;
        Type type = (Type) ((op2 & 0x30) >> 4);
        uint32_t v1 = state.read_register(reg1, type);
        bool write_enable = true;
        uint32_t wb;
        uint32_t smask = sign_mask(type);
        switch(op1) {
            case INCX:
                wb = v1+1;
                next.write_flag(FLAG_C, !(wb & type_mask(type)));
                next.write_flag(FLAG_V, (smask & (wb ^ v1)));
                break;
            case DECX:
                wb = v1-1;
                next.write_flag(FLAG_C, !v1);
                next.write_flag(FLAG_V, (smask & (wb ^ v1)));
                break;
            case TSTX:
                wb = v1;
                write_enable = false;
            case COMX:
                wb = ~v1;
                break;
            case NEGX:
                wb = neg_value(type, v1);
                break;
            case ABSX:
                wb = abs_value(type, v1);
                break;
            case SXTX:
                if(type == TYPE_BYTE) {
                    wb = v1;
                    break;
                }
                wb = sxt_value((Type) (type - 1), v1);
                break;
            case ZXTX:
                if(type == TYPE_BYTE) {
                    wb = v1;
                    break;
                }
                wb = zxt_value((Type) (type - 1), v1);
                break;
            case SHLX:
                wb = v1 << 1;
                next.write_flag(FLAG_C, (smask & v1 & ~wb));
                break;
            case SHRX:
                wb = v1 >> 1;
                next.write_flag(FLAG_C, 0);
                break;
            case ROLX:
                wb = v1 << 1;
                if(smask & v1) wb |= 0x01;
                next.write_flag(FLAG_C, (smask & v1 & ~wb));
                break;
            case RORX:
                wb = v1 >> 1;
                if(wb & 0x01) wb |= smask;
                next.write_flag(FLAG_C, (0x01 & v1));
                break;
        }
        if(write_enable) next.write_register(reg1, type, wb);
        next.write_flag(FLAG_S, smask & wb);
        next.write_flag(FLAG_Z, !(wb & type_mask(type)));
    } else { // fgrp
    }
    return next;
}

Delta BCpu::decode() {
    uint8_t  op1 = nbr->readb(state.pc);

    if(op1 <= 0x0F) return decode_control(op1);
    else if(op1 <= 0x5F) return decode_move(op1);
    else if(op1 <= 0x7F) return decode_jump(op1);
    else return decode_arithmetic(op1);
}

void BCpu::apply(Delta e) {
    state = e.next;
    switch(e.wb_type) {
        case TYPE_NONE: break;
        case TYPE_BYTE:
            nbr->writeb(e.wb_addr, e.wb_value);
            break;
        case TYPE_WORD:
            nbr->writew(e.wb_addr, e.wb_value);
            break;
        case TYPE_LONG:
        case TYPE_FLOAT:
            nbr->writel(e.wb_addr, e.wb_value);
            break;

    }
    //TODO: memory writeback
}

void BCpu::clk() {
    op_wait--;
    if(op_wait <= 0) {
        apply(next);
        next = decode();
    }
}
