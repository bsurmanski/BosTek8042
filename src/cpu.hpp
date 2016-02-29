#ifndef _BOSTEK_CPU_HPP
#define _BOSTEK_CPU_HPP

#include "float16.hpp"
#include "memory.hpp"

#include <stdint.h>

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
 *  P - Parity
 *  H - Half Carry
 *  C - Carry
 */

class Cpu {
    protected:
    uint16_t pc;
    uint16_t sp;

    uint8_t flag; // SZVI TPHC
    uint8_t reg[17]; // 8-bit and 16-bit registers
    float16_t freg[4];

    Memory *mem;

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
        FLAG_P=2,
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

    Cpu(Memory *mem);
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
