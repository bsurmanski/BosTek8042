#ifndef _BOSTEK_CPU_HPP
#define _BOSTEK_CPU_HPP

#include "float16.hpp"
#include "object.hpp"

#include <stdint.h>

class NorthBridge;

#define MAX_BREG 7
#define MAX_WREG 8
#define MAX_FREG 4

#define REGW_HL 0

/*
 * Register formats
 *
 * 8-bit HL, BC, DE overlap with 16-bit R0, R1, R2 respectively
 *
 *  15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
 *  |- -- -- PCH-- -- -- --|-- -- -- -- PCL-- -- -| PC
 *  |- -- -- SPH-- -- -- --|-- -- -- -- SPL-- -- -| SP
 *
 *  |-- -- --FLAG- -- -- --|-- -- -- -- Acc-- -- -| FA
 *  |- -- -- H- -- -- -- --|-- -- -- -- L- -- -- -| R0  [16] indirect
 *
 *                      [8842]
 *  |- -- -- B- -- -- -- --|-- -- -- -- C- -- -- -| R1  [16]
 *  |- -- -- D- -- -- -- --|-- -- -- -- E- -- -- -| R2  [16]
 *
 *                      [80142]
 *  |- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| R3  [16]
 *  |- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| R4  [16]
 *  |- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| R5  [16]
 *  |- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| R6  [16]
 *  |- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| R7  [16]
 *
 *                      [80143]
 *  |- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| F0  [16]
 *  |- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| F1  [16]
 *  |- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| F2  [16]
 *  |- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| F3  [16]
 *
 *  Flag Register Format:
 *  S - Sign
 *  Z - Zero
 *  V - Overflow
 *  I - Interrupt Enable
 *  T - Trap flag
 *  F - Float Finite. High if float result was not NAN or +-INF
 *  H - Half Carry
 *  C - Carry
 */

class Cpu : public Object {
    protected:
    uint16_t pc;
    uint16_t sp;

    uint8_t flag; // SZVI TPHC
    uint8_t reg[17]; // 8-bit and 16-bit registers
    float16_t freg[4];

    NorthBridge *nbr;

    uint8_t readb(uint16_t addr);
    uint16_t readw(uint16_t addr);
    void writeb(uint16_t addr, uint8_t v);
    void writew(uint16_t addr, uint16_t v);

    void pushb(uint8_t v);
    void pushw(uint16_t v);
    void pushf(float16_t v);
    uint8_t pullb();
    uint16_t pullw();
    float16_t pullf();

    void jump(uint16_t addr);
    void branch(int8_t offset);

    void execute_control(uint8_t op);
    void execute_swap8(uint8_t op);
    void execute_load8(uint8_t op);
    void execute_store8(uint8_t op);
    void execute_transfer8(uint8_t op);
    void execute_pushpull8(uint8_t op);
    void execute_memory_rf(uint8_t op);
    void execute_minmax(uint8_t op);
    void execute_branch(uint8_t op);
    void execute_interrupt(uint8_t op);
    void execute_arithmetic8(uint8_t op);
    void execute_arithmetic16(uint8_t op);
    void execute_float(uint8_t op);

    public:
    enum Flag {
        FLAG_S=7,
        FLAG_Z=6,
        FLAG_V=5,
        FLAG_I=4,
        FLAG_T=3,
        FLAG_F=2,
        FLAG_H=1,
        FLAG_C=0
    };

    enum BReg {
        BREG_ACC=0,
        BREG_H,
        BREG_L,
        BREG_B,
        BREG_C,
        BREG_D,
        BREG_E,
        BREG_M,
    };

    enum WReg {
        WREG_HL=0,
        WREG_0=0,
        WREG_1,
        WREG_2,
        WREG_3,
        WREG_4,
        WREG_5,
        WREG_6,
        WREG_7
    };

    enum Operations {
        NOP=0x00, HLT, SFI, CFI, SFT, CFT, BDS, TRP,
        EXR=0x08, SWAH, SWAL, SWAB, SWAC, SWAD, SWAE, SWAM,

        LDMAK=0x10, LDMHK, LDMLK, LDMBK, LDMCK, LDMDK, LDMEK, LDMMK,
        STMKA=0x18, STMKH, STMKL, STMKB, STMKC, STMKD, STMKE, STMKM,

        TRAK=0x20,

        LDMRK=0x40, STMKR, PULR, PSHR, LDMFK, STMKF, PULF, PSHF,

        JMPK=0x68, JMPM,
        //JNE, JEQ, JLT, JLE, JGT, JGE,
        JSRK=0x6A, JSRM,
        RET=0x6B, RFI, IRQ, NMI, WFI,

        JCC=0x70, JHC, JPC, JTC, JIC, JVC, JZC, JSC,
        JCS=0x78, JHS, JPS, JTS, JIS, JVS, JZS, JSS,

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

    Cpu();
    ~Cpu();

    uint8_t read_breg(uint8_t r);
    uint16_t read_wreg(uint8_t r);
    float16_t read_freg(uint8_t r);

    void write_breg(uint8_t r, uint8_t v);
    void write_wreg(uint8_t r, uint16_t v);
    void write_freg(uint8_t r, float16_t v);

    void nop();
    void hlt();

    bool get_flag(Flag b);
    void set_flag(Flag b, bool v);

    void clk();

    friend class NorthBridge;

#ifndef FRIEND_TEST
#define FRIEND_TEST(X, Y)
#endif
    friend class CpuTest;
    FRIEND_TEST(CpuTest, PushPull);
    FRIEND_TEST(CpuTest, Swap8);
    FRIEND_TEST(CpuTest, Transfer8);
    FRIEND_TEST(CpuTest, PushPull8);
    FRIEND_TEST(CpuTest, Jump);
    FRIEND_TEST(CpuTest, Arithmetic8);
};

#endif
