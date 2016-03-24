#include "bcpu.hpp"
#include "bcpu_opcodes.hpp"

#include <string.h>

#include <stdio.h>


uint32_t BCpu::sign_mask(Type ty) {
    switch(ty) {
        case NONE:
        case BYTE: return 0x80;
        case WORD: return 0x8000;
        case FLOAT:
        case LONG: return 0x80000000;
    }
    return 0;
}

uint32_t BCpu::type_mask(Type ty) {
    switch(ty) {
        case NONE:
        case BYTE: return 0xFF;
        case WORD: return 0xFFFF;
        case FLOAT:
        case LONG: return 0xFFFFFFFF;
    }
    return 0;
}

bool BCpu::is_negative(Type ty, uint32_t val) {
    return sign_mask(ty) & val;
}

uint32_t BCpu::zxt_value(Type ty, uint32_t val) {
    if(ty == BYTE) return val;
    return val & type_mask(ty);
}

uint32_t BCpu::sxt_value(Type ty, uint32_t val) {
    if(is_negative(ty, val)) {
        return (type_mask(LONG) ^ type_mask(ty)) | val;
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

BCpu::Effect::Effect() : wb_type(NONE) {
}

BCpu::Effect::Effect(State s) : next(s), wb_type(NONE) {
}

BCpu::Effect::Effect(State s, Type ty, uint32_t addr, uint32_t v) : next(s), wb_type(ty), wb_addr(addr), wb_value(v) {
}

BCpu::BCpu() {
}

BCpu::BCpu(uint32_t pc, uint32_t sp) {
    state.pc = pc;
    state.sp = sp;
}

BCpu::State::State() : pc(0), sp(0), sb(0) {
    memset(registers, 0, sizeof(registers));
    memset(fregisters, 0, sizeof(fregisters));
}

uint32_t BCpu::State::read_register(uint8_t reg, uint8_t type) {
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

void BCpu::State::write_register(uint8_t reg, uint8_t type, uint32_t val) {
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

uint8_t BCpu::State::readb_register(uint8_t reg) {
    if(reg < 4) {
        return registers[reg] & 0xFF;
    } else if(reg < 8) {
        return (registers[reg-4] & 0xFF00) >> 8;
    }
    return 0;
}

void BCpu::State::writeb_register(uint8_t reg, uint8_t val) {
    if(reg < 4) {
        registers[reg] = (registers[reg] & 0xFFFFFF00) | val;
    } else if(reg < 8) {
        registers[reg-4] = (registers[reg-4] & 0xFFFF00FF) | (val << 8);
    }
}

uint16_t BCpu::State::readw_register(uint8_t reg) {
    if(reg < 4) {
        return registers[reg] & 0xFFFF;
    } else if(reg < 8) {
        return (registers[reg-4] & 0xFFFF0000) >> 16;
    }
    return 0;
}

void BCpu::State::writew_register(uint8_t reg, uint16_t val) {
    if(reg < 4) {
        registers[reg] = (registers[reg] & 0xFFFF0000) | val;
    } else if(reg < 8) {
        registers[reg-4] = (registers[reg-4] & 0x0000FFFF) | (val << 16);
    }
}

uint32_t BCpu::State::readl_register(uint8_t reg) {
    if(reg < 4) {
        return registers[reg];
    }
    return 0;
}

void BCpu::State::writel_register(uint8_t reg, uint32_t val) {
    if(reg < 4) {
        registers[reg] = val;
    }
}

float BCpu::State::readf_register(uint8_t reg) {
    if(reg < 4) {
        return fregisters[reg];
    }
    return 0;
}

void BCpu::State::writef_register(uint8_t reg, float val) {
    if(reg < 4) {
        fregisters[reg] = val;
    }
}

bool BCpu::State::read_flag(Flag f) {
    return (bool) (sb & (0x01 << ((uint8_t) f)));
}

void BCpu::State::write_flag(Flag f, bool b) {
    sb = (sb & ~(0x01 << (uint8_t) f)) | (b << (uint8_t) f);
}

BCpu::Effect BCpu::decode_control(uint8_t op1) {
    BCpu::State next(state);
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

BCpu::Effect BCpu::decode_move(uint8_t op1) {
    BCpu::State next(state);
    Type wb_type = NONE; // used for PSH/POP
    uint32_t wb_addr = 0; // used for PSH/POP
    uint32_t wb_value = 0; // used for PSH/POP

    if(op1 <= LSTOF_RRK) { // load/store
        //TODO
    } else if(op1 <= MOVB_CR) { // move, load constant
        bool use_constant = op1 & 0x04;
        uint8_t op2 = nbr->readb(state.pc+1);
        uint8_t type = op1 & 0x03;
        uint32_t val;
        if(use_constant) { // move in constant
            switch(type) {
                case BYTE:
                    val = nbr->readb(state.pc+2);
                    next.pc+=3;
                    break;
                case WORD:
                    val = nbr->readw(state.pc+2);
                    next.pc+=4;
                    break;
                case LONG:
                case FLOAT:
                    val = nbr->readl(state.pc+2);
                    next.pc+=6;
                    break;
            }
        } else { // move from register
            val = state.read_register((op2 & 0xF0) >> 4, type);
            next.pc+=2;
        }
        next.write_register(op2 & 0x0F, type, val);
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
        uint8_t type = op2 & 0x30;

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
                case PSHX_C:
                    v = state.sb;
                    next.pc += 2;
                    break;
                case PSHX_K:
                    switch(type) {
                        case BYTE:
                            v = nbr->readb(state.pc+2);
                            next.pc += 3;
                            break;
                        case WORD:
                            v = nbr->readw(state.pc+2);
                            next.pc += 4;
                            break;
                        case LONG:
                        case FLOAT:
                            v = nbr->readw(state.pc+2);
                            next.pc += 6;
                            break;
                    }
                    break;
            }

            // write value
            // update sp
            switch(type) {
                case BYTE:
                    next.sp-=1;
                    wb_type = BYTE;
                    wb_addr = next.sp;
                    wb_value = v;
                    break;
                case WORD:
                    next.sp-=2;
                    wb_type = WORD;
                    wb_addr = next.sp;
                    wb_value = v;
                    break;
                case LONG:
                case FLOAT:
                    next.sp-=4;
                    wb_type = LONG;
                    wb_addr = next.sp;
                    wb_value = v;
                    break;
            }
        } else { // else POP
            // read value
            // update sp
            uint32_t v;
            switch(type) {
                case BYTE:
                    v = nbr->readb(next.sp);
                    next.sp+=1;
                    break;
                case WORD:
                    v = nbr->readw(next.sp);
                    next.sp+=2;
                    break;
                case LONG:
                case FLOAT:
                    v = nbr->readl(next.sp);
                    next.sp+=4;
                    break;
            }

            switch(op1) {
                case POPX_R:
                    next.write_register(op2 & 0x0F, type, v);
                    break;
                case POPX_C:
                    next.sb = v & 0x000000FF;
                    break;
                case POPX_X:
                    break;
            }
        }
    }

    return Effect(next, wb_type, wb_addr, wb_value);
}

BCpu::Effect BCpu::decode_jump(uint8_t op1) {
    BCpu::State next(state);
    Type wb_type = NONE; // used for JSR
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
            wb_type = LONG;
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

        if(state.read_flag((BCpu::Flag) (op1 & 0x07)) == (bool)(op1 & 0x08)){
            next.pc += offset;
        }
    }
    return Effect(next, wb_type, wb_addr, wb_value);
}

BCpu::Effect BCpu::decode_arithmetic(uint8_t op1) {
    BCpu::State next(state);
    uint8_t op2 = nbr->readb(state.pc+1);

    if(op1 < 0xF0) { // Binary Arithmetic
        bool imm = op1 & 0x04;
        uint8_t reg1 = op2 & 0x0F;
        BCpu::Type type = (BCpu::Type) (op1 & 0x03);
        uint32_t v1 = state.read_register(reg1, type);
        uint32_t v2;
        uint32_t wb = 0;
        bool write_enable = true;
        uint32_t smask = sign_mask(type);
        bool sign_parity;
        bool mul_overflow = false;

        if(imm) { // uses immediate constant
            switch(type) {
                case NONE:
                case BYTE:
                    v2 = nbr->readb(state.pc+2);
                    next.pc+=3;
                    break;
                case WORD:
                    v2 = nbr->readw(state.pc+2);
                    next.pc+=4;
                    break;
                case LONG:
                    v2 = nbr->readl(state.pc+2);
                    next.pc+=6;
                    break;
                case FLOAT:
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
                break;
            case ADC:
                wb = v1 + v2 + state.read_flag(FLAG_C);
                next.write_flag(FLAG_C, (smask & v1 & v2) || (smask & v1 & ~wb) || (smask & v2 & ~wb));
                break;
            case SUB:
                wb = v1 - v2;
                next.write_flag(FLAG_C, (smask & v1 & v2) || (smask & v1 & ~wb) || (smask & v2 & ~wb));
                break;
            case SBC:
                wb = v1 - v2 - state.read_flag(FLAG_C);
                next.write_flag(FLAG_C, (smask & v1 & v2) || (smask & v1 & ~wb) || (smask & v2 & ~wb));
                break;
            case CMP:
               wb = v1 - v2;
               next.write_flag(FLAG_C, (smask & v1 & v2) || (smask & v1 & ~wb) || (smask & v2 & ~wb));
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
                    mul_overflow = true;
                }
                next.write_flag(FLAG_C, mul_overflow);

                // if only one is negative
                if(smask & (v1 ^ v2)) wb = neg_value(type, wb);
                break;

            case POW:
                if(v2 & smask) {
                    wb = 0;
                    next.write_flag(FLAG_C, false);
                } else {
                    wb = pow_value(abs_value(type, v1), abs_value(type, v2));

                    if ((wb > type_mask(type)) ||
                        (abs_value(type, v1) >= 2 && v2 >= 32)) {
                        mul_overflow = true;
                    } // catch overflow if value WAY overflows
                    next.write_flag(FLAG_C, mul_overflow);
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

                mul_overflow = !v2;  // consider v2 == 0 to overflow
                next.write_flag(FLAG_C, mul_overflow); // only way to carry is v2 == 0

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
        next.write_flag(FLAG_V, (smask & ~wb & v1 & v2) || (smask & wb & ~v1 & ~v2) || mul_overflow);
        next.write_flag(FLAG_Z, !(wb & type_mask(type)));
    } else if(op1 <= RORX) { // unary
        next.pc += 2;
        uint8_t reg1 = op2 & 0x0F;
        BCpu::Type type = (BCpu::Type) ((op2 & 0x30) >> 4);
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
                if(type == BYTE) {
                    wb = v1;
                    break;
                }
                wb = sxt_value((BCpu::Type) (type - 1), v1);
                break;
            case ZXTX:
                if(type == BYTE) {
                    wb = v1;
                    break;
                }
                wb = zxt_value((BCpu::Type) (type - 1), v1);
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

BCpu::Effect BCpu::decode() {
    uint8_t  op1 = nbr->readb(state.pc);

    if(op1 <= 0x0F) return decode_control(op1);
    else if(op1 <= 0x5F) return decode_move(op1);
    else if(op1 <= 0x7F) return decode_jump(op1);
    else return decode_arithmetic(op1);
}

void BCpu::apply(Effect e) {
    state = e.next;
    //TODO: memory writeback
}

void BCpu::clk() {
    op_wait--;
    if(op_wait <= 0) {
        apply(nextEffect);
        nextEffect = decode();
    }
}
