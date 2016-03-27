#ifndef _BOSTEK_BCPU_HPP
#define _BOSTEK_BCPU_HPP

#include "cpu.hpp"

namespace Bostek {
namespace Cpu {
enum OpCodes {
    NOP=0x00, HLT, WFI, RET, RFI, IRQ, NMI, CPUB, ANSB_R=0x08, ORSB_R, XRSB_R, ANSB_K=0x0C, ORSB_K, XRSB_K,
    LODB_RK=0x10, LODW_RK, LODL_RK, LODF_RK, LLODB_RK, LLODW_RK, LLODL_RK, LLODF_RK, STOB_RK, STOW_RK, STOL_RK, STOF_RK, LSTOB_RK, LSTOW_RK, LSTOL_RK, LSTOF_RK,
    LODB_RRK=0x20, LODW_RRK, LODL_RRK, LODF_RRK, LLODB_RRK, LLODW_RRK, LLODL_RRK, LLODF_RRK, STOB_RRK, STOW_RRK, STOL_RRK, STOF_RRK, LSTOB_RRK, LSTOW_RRK, LSTOL_RRK, LSTOF_RRK,
    MOVB_RR=0x30, MOVW_RR, MOVL_RR, MOVF_RR, MOVB_RK, MOVW_RK, MOVL_RK, MOVF_RK, MOVB_SR=0x38, MOVB_RS=0x3A, MOVB_SK=0x3C,
    SWPB=0x40, SWPW, SWPL, SWPF, POPX_R=0x48, POPX_S, POPX_X, PSHX_R=0x4C, PSHX_S, PSHX_K,
    BTOF=0x50, WTOF, LTOF, FTOB=0x54, FTOW, FTOL,
    AJMP=0x60, LAJMP, AJSR, LAJSR, RJMP, LRJMP, RJSR, LRJSR,
    JCC=0x70, JHC, JFC, JTC, JIC, JVC, JZC, JSC, JCS=0x78, JHS, JFS, JTS, JIS, JVS, JZS, JSS,
    ADD=0x80, ADDB_RR=0x80, ADDW_RR, ADDL_RR, ADDF_RR, ADDB_RK, ADDW_RK, ADDL_RK, ADDF_RK,
    ADC = 0x88, ADCB_RR=0x88, ADCW_RR, ADCL_RR, ADCF_RR, ADCB_RK, ADCW_RK, ADCL_RK, ADCF_RK,
    SUB = 0x90, SUBB_RR=0x90, SUBW_RR, SUBL_RR, SUBF_RR, SUBB_RK, SUBW_RK, SUBL_RK, SUBF_RK,
    SBC = 0x98, SBCB_RR=0x98, SBCW_RR, SBCL_RR, SBCF_RR, SBCB_RK, SBCW_RK, SBCL_RK, SBCF_RK,
    CMP = 0xA0, CMPB_RR=0xA0, CMPW_RR, CMPL_RR, CMPF_RR, CMPB_RK, CMPW_RK, CMPL_RK, CMPF_RK,
    AND = 0xA8, ANDB_RR=0xA8, ANDW_RR, ANDL_RR, ANDF_RR, ANDB_RK, ANDW_RK, ANDL_RK, ANDF_RK,
    OR = 0xB0, ORB_RR=0xB0, ORW_RR, ORL_RR, ORF_RR, ORB_RK, ORW_RK, ORL_RK, ORF_RK,
    XOR = 0xB8, XORB_RR=0xB8, XORW_RR, XORL_RR, XORF_RR, XORB_RK, XORW_RK, XORL_RK, XORF_RK,
    MUL = 0xC0, MULB_RR=0xC0, MULW_RR, MULL_RR, MULF_RR, MULB_RK, MULW_RK, MULL_RK, MULF_RK,
    DIV = 0xC8, DIVB_RR=0xC8, DIVW_RR, DIVL_RR, DIVF_RR, DIVB_RK, DIVW_RK, DIVL_RK, DIVF_RK,
    MOD = 0xD0, MODB_RR=0xD0, MODW_RR, MODL_RR, MODF_RR, MODB_RK, MODW_RK, MODL_RK, MODF_RK,
    POW = 0xD8, POWB_RR=0xD8, POWW_RR, POWL_RR, POWF_RR, POWB_RK, POWW_RK, POWL_RK, POWF_RK,
    MIN = 0xE0, MINB_RR=0xE0, MINW_RR, MINL_RR, MINF_RR, MINB_RK, MINW_RK, MINL_RK, MINF_RK,
    MAX = 0xE8, MAXB_RR=0xE8, MAXW_RR, MAXL_RR, MAXF_RR, MAXB_RK, MAXW_RK, MAXL_RK, MAXF_RK,
    INCX=0xF0, DECX, TSTX, COMX, NEGX, ABSX, SXTX, ZXTX, SHLX, SHRX, ROLX, RORX, FGRP1,
};


enum Type {
    TYPE_BYTE=0,
    TYPE_WORD=1,
    TYPE_LONG=2,
    TYPE_FLOAT=3,
    TYPE_NONE=4,
};

enum Flag {
    FLAG_C=0,
    FLAG_H,
    FLAG_F,
    FLAG_T,
    FLAG_I,
    FLAG_V,
    FLAG_Z,
    FLAG_S,
};

enum FlagBit {
    FLAGBIT_C=0x01,
    FLAGBIT_H=0x02,
    FLAGBIT_F=0x04,
    FLAGBIT_T=0x08,
    FLAGBIT_I=0x10,
    FLAGBIT_V=0x20,
    FLAGBIT_Z=0x40,
    FLAGBIT_S=0x80,
};

enum Register {
    REG_A=0,
    REG_B,
    REG_C,
    REG_D,
    REG_AH,
    REG_BH,
    REG_CH,
    REG_DH,
};

struct State {
    public:
    uint32_t pc; // program counter
    uint32_t sp; // stack pointer
    uint8_t sb; // status byte (flags)

    uint32_t registers[4];
    float fregisters[4];

    uint32_t read_register(uint8_t reg, uint8_t type);
    void write_register(uint8_t reg, uint8_t type, uint32_t val);

    uint8_t readb_register(uint8_t reg);
    void writeb_register(uint8_t reg, uint8_t val);
    uint16_t readw_register(uint8_t reg);
    void writew_register(uint8_t reg, uint16_t val);
    uint32_t readl_register(uint8_t reg);
    void writel_register(uint8_t reg, uint32_t val);
    float readf_register(uint8_t reg);
    void writef_register(uint8_t reg, float val);

    bool read_flag(Flag f);
    void write_flag(Flag f, bool b);
    bool flags_set(uint8_t flags);
    bool flags_clear(uint8_t flags);

    State();
};

struct Delta {
    public:
    State next;

    Type wb_type;
    uint32_t wb_addr;
    uint32_t wb_value;

    Delta();
    Delta(State s);
    Delta(State s, Type ty, uint32_t addr, uint32_t v);
};

class BCpu : public ::Cpu {
    uint32_t sign_mask(Type ty);
    uint32_t type_mask(Type ty);
    bool is_negative(Type ty, uint32_t val);
    uint32_t zxt_value(Type ty, uint32_t val);
    uint32_t sxt_value(Type ty, uint32_t val);
    uint32_t neg_value(Type ty, uint32_t val);
    uint32_t abs_value(Type ty, uint32_t val);
    uint64_t pow_value(uint32_t v1, uint32_t v2); // assumes unsigned

    public:
    State state;
    Delta next;

    int op_wait; // how many clks to wait until commiting

    Delta decode_control(uint8_t op1);
    Delta decode_move(uint8_t op1);
    Delta decode_jump(uint8_t op1);
    Delta decode_arithmetic(uint8_t op1);
    Delta decode();
    void apply(Delta e);

    public:
    BCpu();
    BCpu(uint32_t pc, uint32_t sp);
    virtual void clk();

    friend class BCpuTest;
};

}
}

#endif
