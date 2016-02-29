#include "cpu.hpp"

enum Operations {
    NOP=0x00, HLT, SFI, CFI, SFT, CFT, BDS, TRP,
    EXR=0x08, SWAH, SWAL, SWAB, SWAC, SWAD, SWAE, SWAM,

    LDMAK=0x10, LDMHK, LDMLK, LDMBK, LDMCK, LDMDK, LDMEK, LDMMK,
    STMKA=0x18, STMKH, STMKL, STMKB, STMKC, STMKD, STMKE, STMKM,

    TRAK=0x20,

    LDMRK=0x40, STMKR, PULR, PSHR, LDMFK, STMKF, PULF, PSHF,

    JCC=0x60, JHC, JPC, JTC, JIC, JVC, JZC, JSC,
    JCS=0x68, JHS, JPS, JTS, JIS, JVS, JZS, JSS,

    JMPK=0x70, JMPM, JNE, JEQ, JLT, JLE, JGT, JGE,
    JSRK=0x78, JSRM,
    RET=0x7B, RFI, IRQ, NMI, WFI,

    ADDK=0x80, ADDH, ADDL, ADDB, ADDC, ADDD, ADDE, ADDM,
    ADCK=0x88, ADCH, ADCL, ADCB, ADCC, ADCD, ADCE, ADCM,

    SUBK=0x90, SUBH, SUBL, SUBB, SUBC, SUBD, SUBE, SUBM,
    SBCK=0x98, SBCH, SBCL, SBCB, SBCC, SBCD, SBCE, SBCM,

    CMPK=0xA0, CMPH, CMPL, CMPB, CMPC, CMPD, CMPE, CMPM,
    ANDK=0xA8, ANDH, ANDL, ANDB, ANDC, ANDD, ANDE, ANDM,

    ORRK=0xB0, ORRH, ORRL, ORRB, ORRC, ORRD, ORRE, ORRM,
    XORK=0xB8, XORH, XORL, XORB, XORC, XORD, XORE, XORM,

    SHL=0xC0, SHR, ROL, ROR, COM, NEG,
};

Cpu::Cpu(Memory *_mem) {
    pc = 0x0000;
    sp = 0x1000;
    mem = _mem;
}

Cpu::~Cpu() {
}

void Cpu::pushb(uint8_t v) {
    mem->writeb(--sp, v);
}

void Cpu::pushw(uint16_t v) {
    sp-=2;
    mem->writew(sp, v);
}

void Cpu::pushf(float16_t v) {
    sp-=2;
    mem->writew(sp, v.packed());
}

uint8_t Cpu::pullb() {
    return mem->readb(sp++);
}

uint16_t Cpu::pullw() {
    uint16_t ret = mem->readw(sp);
    sp+=2;
    return ret;
}

float16_t Cpu::pullf() {
    float16_t ret = float16_t::unpack(mem->readw(sp++));
    sp+=2;
    return ret;
}

void Cpu::jump(uint16_t addr) {
    pc = addr;
}

void Cpu::branch(int8_t offset) {
    pc += offset;
}

uint8_t Cpu::read_breg(uint8_t r) {
    if(r > MAX_BREG) return 0xFF;
    if(r == BREG_M) return mem->readb(read_wreg(REGW_HL));
    return reg[r];
}

uint16_t Cpu::read_wreg(uint8_t r) {
    if(r > MAX_WREG) return 0xFFFF;
    return (reg[r*2+1]<<8) | (reg[r*2+2]);
}

float16_t Cpu::read_freg(uint8_t r) {
    if(r > MAX_FREG) return float16_t(); // TODO: NAN
    return freg[r];
}

void Cpu::write_breg(uint8_t r, uint8_t v) {
    if(r > MAX_BREG) return;
    if(r == BREG_M) mem->writeb(read_wreg(REGW_HL), v);
    reg[r] = v;
}

void Cpu::write_wreg(uint8_t r, uint16_t v) {
    if(r > MAX_WREG) return;
    reg[r*2+1] = ((v&0xFF00) >> 8);
    reg[r*2+2] = v&0x00FF;
}

void Cpu::write_freg(uint8_t r, float16_t v) {
    if(r > MAX_FREG) return;
    freg[r] = v;
}

void Cpu::nop() {
    pc++;
}

void Cpu::hlt() {
}

bool Cpu::get_flag(Flag b) {
    return (flag >> b) & 0x0001;
}

void Cpu::set_flag(Flag b, bool v) {
    uint16_t mask = 0x0001 << b;
    if(v) flag |= mask;
    else flag &= (~mask);
}

void Cpu::execute_control(uint8_t op) {
    switch(op) {
        case NOP: break;
        case HLT: hlt(); break;
        case SFI: set_flag(FLAG_I, true); break;
        case CFI: set_flag(FLAG_I, false); break;
        case SFT: set_flag(FLAG_T, true); break;
        case CFT: set_flag(FLAG_T, false); break;
        case BDS: /*TODO: explode*/ break;
        case TRP: /*TODO*/break;
        case EXR: /*TODO*/break;
    }
}

void Cpu::execute_swap8(uint8_t op) {
    uint8_t reg = op & 0x07;
    uint8_t tmp = read_breg(reg);
    write_breg(reg, read_breg(BREG_ACC));
    write_breg(BREG_ACC, tmp);
    pc++;
}

void Cpu::execute_load8(uint8_t op) {
    uint16_t addr = mem->readw(++pc);
    uint8_t reg = op & 0x07;
    write_breg(reg, mem->readb(addr));
    pc+=2;
}

void Cpu::execute_store8(uint8_t op) {
    uint16_t addr = mem->readw(++pc);
    uint8_t reg = op & 0x07;
    mem->writeb(addr, read_breg(reg));
    pc+=2;
}

void Cpu::execute_transfer8(uint8_t op) {
    if(op == TRAK) {
        uint8_t k = mem->readb(++pc);
        write_breg(BREG_ACC, k);
    } else if(op <= 0x27) { // TRA
        uint8_t reg = op & 0x07;
        write_breg(BREG_ACC, read_breg(reg));
    } else { // TAR
        uint8_t reg = op & 0x07;
        write_breg(reg, read_breg(BREG_ACC));
    }
    pc++;
}

void Cpu::execute_pushpull8(uint8_t op) {
    uint8_t reg = op & 0x07;
    if(op <= 0x37) { // pull
        write_breg(reg, pullb());
    } else { // push
        pushb(read_breg(reg));
    }
    pc++;
}

void Cpu::execute_memory_rf(uint8_t op) {
    //TODO
}

void Cpu::execute_minmax(uint8_t op) {
    //TODO
}

void Cpu::execute_branch(uint8_t op) {
    if(op <= 0x6F) { // jump on flag
        uint16_t addr = mem->readw(++pc);
        pc+=2;
        bool sc = (op & 0x08); // set/clear
        uint8_t flg = op & 0x07;
        if(get_flag((Flag) flg) == sc) {
            jump(addr); // jump
        }
    } else if(op <= 0x79) { // jump, jump on condition
        uint16_t addr;
        pc++;
        if(op == JMPM || op == JSRM) {
            addr = read_wreg(WREG_HL);
        } else {
            addr = mem->readw(pc);
            pc+=2;
        }

        switch(op) {
            case JSRK:
            case JSRM:
                pushw(pc); // push return address
                //fallthrough
            case JMPK:
            case JMPM:
                jump(addr); break;
            case JNE: if(!get_flag(FLAG_Z)) jump(addr); break;
            case JEQ: if(get_flag(FLAG_Z)) jump(addr); break;
            case JLT: if(get_flag(FLAG_S)) jump(addr); break;
            case JLE: if(get_flag(FLAG_S)||get_flag(FLAG_Z)) jump(addr); break;
            case JGT: if(!get_flag(FLAG_S)) jump(addr); break;
            case JGE: if(!get_flag(FLAG_S)||get_flag(FLAG_Z)) jump(addr); break;
        }
    } else if(op == RET || op == RFI) {
        uint16_t addr = pullw();
        jump(addr);
        if(op == RFI) {
            set_flag(FLAG_I, true); // re-enable interrupts
        }
    }

    //TODO: IRQ, NMI, WFI
}

void Cpu::execute_interrupt(uint8_t op) {
    //TODO
}

void Cpu::execute_arithmetic8(uint8_t op) {
    pc++;
    uint8_t acc = read_breg(BREG_ACC);
    if(op <= 0xBF) { // binary op
        uint8_t reg = op & 0x07;
        uint8_t val;
        if(reg == 0) { // actually use constant, not reg
            val = mem->readb(pc++);
        } else {
            val = read_breg(reg);
        }

        //TODO: set flags
        if(op <= 0x87) { // ADD
            write_breg(BREG_ACC, acc + val);
        } else if(op <= 0x8F) { // ADC
            write_breg(BREG_ACC, acc + val + get_flag(FLAG_C));
        } else if(op <= 0x97) { // SUB
            write_breg(BREG_ACC, acc - val);
        } else if(op <= 0x9F) { // SBC
            write_breg(BREG_ACC, acc - val - get_flag(FLAG_C));
        } else if(op <= 0xA7) { // CMP
            //TODO
        } else if(op <= 0xAF) { // AND
            write_breg(BREG_ACC, acc & val);
        } else if(op <= 0xB7) { // ORR
            write_breg(BREG_ACC, acc | val);
        } else { // XOR
            write_breg(BREG_ACC, acc ^ val);
        }
    } else { // unary op
        //TODO: flags
        switch(op) {
            case SHL:
                set_flag(FLAG_C, acc & 0x80); // carry <- high bit
                write_breg(BREG_ACC, acc << 1);
                break;
            case SHR:
                set_flag(FLAG_C, acc & 0x01); // carry <- low bit
                write_breg(BREG_ACC, acc >> 1);
                break;
            case ROL:
                set_flag(FLAG_C, acc & 0x80); // carry <- high bit
                write_breg(BREG_ACC, (acc << 1) | (acc >> 7));
                break;
            case ROR:
                set_flag(FLAG_C, acc & 0x01); // carry <- low bit
                write_breg(BREG_ACC, (acc >> 1) | (acc << 7));
                break;
            case COM:
                write_breg(BREG_ACC, ~acc);
                set_flag(FLAG_S, acc & 0x80);
                set_flag(FLAG_Z, !acc);
                break;
            case NEG:
                write_breg(BREG_ACC, (~acc) + 1);
                break;
        }
    }
}

void Cpu::execute_arithmetic16(uint8_t op) {
    //TODO
}

void Cpu::execute_float(uint8_t op) {
    //TODO
}

void Cpu::clk() {
    uint8_t op = mem->readb(pc);
    if(op <= 0x08) execute_control(op);
    else if(op <= 0x0F) execute_swap8(op);
    else if(op <= 0x17) execute_load8(op);
    else if(op <= 0x1F) execute_store8(op);
    else if(op <= 0x2F) execute_transfer8(op);
    else if(op <= 0x3F) execute_pushpull8(op);
    else if(op <= 0x52) execute_memory_rf(op);
    else if(op <= 0x5F) execute_minmax(op);
    else if(op <= 0x7C) execute_branch(op);
    else if(op <= 0x7F) execute_interrupt(op);
    else if(op <= 0xC5) execute_arithmetic8(op);
    else if(op <= 0xE1) execute_arithmetic16(op);
    else execute_float(op);
}
