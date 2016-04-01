#include <gtest/gtest.h>

#include "../src/bcpu.hpp"
#include "../src/memory.hpp"
#include "../src/northBridge.hpp"

namespace Bostek {
namespace Cpu {

class TestOp {
    public:
    int len;
    uint8_t ops[6];

    TestOp(uint8_t op0) {
        len = 1;
        ops[0] = op0;
    }

    TestOp(uint8_t op0, uint8_t op1) {
        len = 2;
        ops[0] = op0;
        ops[1] = op1;
    }

    TestOp(uint8_t op0, uint8_t op1, uint8_t op2) {
        len = 3;
        ops[0] = op0;
        ops[1] = op1;
        ops[2] = op2;
    }

    TestOp(uint8_t op0, uint8_t op1, uint8_t op2, uint8_t op3) {
        len = 4;
        ops[0] = op0;
        ops[1] = op1;
        ops[2] = op2;
        ops[3] = op3;
    }

    TestOp(uint8_t op0, uint8_t op1, uint8_t op2, uint8_t op3, uint8_t op4, uint8_t op5) {
        len = 6;
        ops[0] = op0;
        ops[1] = op1;
        ops[2] = op2;
        ops[3] = op3;
        ops[4] = op4;
        ops[5] = op5;
    }
};

class BCpuTest : public testing::Test {
    public:
    NorthBridge *nbr;
    Memory *mem;
    BCpu *cpu;

    virtual void SetUp() {
        mem = new Memory(0x2FFFF);
        cpu = new BCpu(0x1000, 0x1000);
        nbr = new NorthBridge;
        nbr->attachCpu(cpu);
        nbr->attachMemory(mem);
    }

    virtual void TearDown() {
        delete nbr;
    }

    Delta Execute(TestOp op) {
        mem->fill(cpu->state.pc, op.len, op.ops);
        Delta e = cpu->decode();
        EXPECT_EQ(e.next.pc, cpu->state.pc + op.len);
        cpu->apply(e);
        return e;
    }

    Delta ExecuteJump(TestOp op) {
        mem->fill(cpu->state.pc, op.len, op.ops);
        Delta e = cpu->decode();
        cpu->apply(e);
        return e;
    }
};

TEST_F(BCpuTest, ORSB_ANSB_XRSB) {
    Delta e;

    cpu->state.sb = 0x00;

    e = Execute(TestOp(ORSB_K, 0x03));
    EXPECT_EQ(e.next.sb, 0x03);

    e = Execute(TestOp(ORSB_K, 0x05));
    EXPECT_EQ(e.next.sb, 0x07);

    e = Execute(TestOp(ANSB_K, 0x02));
    EXPECT_EQ(e.next.sb, 0x02);

    e = Execute(TestOp(XRSB_K, 0x06));
    EXPECT_EQ(e.next.sb, 0x04);

    Execute(TestOp(MOVB_RK, REG_B, 0xFF));

    e = Execute(TestOp(XRSB_R, REG_B));
    EXPECT_EQ(e.next.sb, 0xFB);

    e = Execute(TestOp(ORSB_R, REG_B));
    EXPECT_EQ(e.next.sb, 0xFF);

    Execute(TestOp(MOVB_RK, REG_B, 0x50));

    e = Execute(TestOp(ANSB_R, REG_B));
    EXPECT_EQ(e.next.sb, 0x50);
}

TEST_F(BCpuTest, LOD) {
    uint8_t test_data[] = {
        0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xff
    };

    mem->fill(0x2000, sizeof(test_data), test_data);

    Delta e;
    e = Execute(TestOp(LODB_RK, 0x01, 0xfc, 0x0f));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x12);

    e = Execute(TestOp(LODB_RK, 0x01, 0xfc, 0x0f));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x9a);

    e = Execute(TestOp(LODW_RK, 0x01, 0xf8, 0x0f));
    EXPECT_EQ(e.next.readw_register(REG_B), 0xbc9a);

    e = Execute(TestOp(LODL_RK, 0x01, 0xf4, 0x0f));
    EXPECT_EQ(e.next.readl_register(REG_B), 0xffdebc9a);

    cpu->state.writew_register(REG_C, 0x03);
    e = Execute(TestOp(LODB_RRK, 0x21, 0xec, 0x0f));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x78);

    // offset is always word
    cpu->state.writel_register(REG_C, 0x12340004);
    e = Execute(TestOp(LODB_RRK, 0x21, 0xe8, 0x0f));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x9a);

    // test negative
    cpu->state.writel_register(REG_C, 0x1234FFDC);
    cpu->state.pc = 0x2020;
    e = Execute(TestOp(LODW_RRK, 0x21, 0x00, 0x00));
    EXPECT_EQ(e.next.readw_register(REG_B), 0x3412);
}

TEST_F(BCpuTest, STO) {
    Delta e;
    cpu->state.writew_register(REG_B, 0x1234);
    e = Execute(TestOp(STOB_RK, 0x01, 0xfc, 0x0f));
    EXPECT_EQ(e.wb_type, Bostek::Cpu::TYPE_BYTE);
    EXPECT_EQ(e.wb_addr, 0x2000);
    EXPECT_EQ(e.wb_value, 0x34);

    cpu->state.writew_register(REG_B, 0x5678);
    e = Execute(TestOp(STOW_RK, 0x01, 0xfc, 0x0f));
    EXPECT_EQ(e.wb_type, Bostek::Cpu::TYPE_WORD);
    EXPECT_EQ(e.wb_addr, 0x2004);
    EXPECT_EQ(e.wb_value, 0x5678);

    cpu->state.writel_register(REG_B, 0x12345678);
    e = Execute(TestOp(STOL_RK, 0x01, 0xfc, 0x0f));
    EXPECT_EQ(e.wb_type, Bostek::Cpu::TYPE_LONG);
    EXPECT_EQ(e.wb_addr, 0x2008);
    EXPECT_EQ(e.wb_value, 0x12345678);

    cpu->state.writel_register(REG_C, 0xFFFF0002);
    cpu->state.writel_register(REG_B, 0xABCD);
    e = Execute(TestOp(STOW_RRK, 0x21, 0xfc, 0x0f));
    EXPECT_EQ(e.wb_type, Bostek::Cpu::TYPE_WORD);
    EXPECT_EQ(e.wb_addr, 0x200E);
    EXPECT_EQ(e.wb_value, 0xABCD);

    cpu->state.writew_register(REG_C, 0xFFFA);
    cpu->state.writel_register(REG_B, 0xABCD);
    e = Execute(TestOp(STOW_RRK, 0x21, 0xfc, 0x0f));
    EXPECT_EQ(e.wb_type, Bostek::Cpu::TYPE_WORD);
    EXPECT_EQ(e.wb_addr, 0x200A);
    EXPECT_EQ(e.wb_value, 0xABCD);
}

TEST_F(BCpuTest, MOV) {
    Delta e;
    e = Execute(TestOp(MOVB_RK, REG_A, 0x21));
    EXPECT_EQ(e.next.readb_register(REG_A), 0x21);

    e = Execute(TestOp(MOVB_RK, REG_B, 0x55));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x55);

    e = Execute(TestOp(MOVB_RR, REG_B | (REG_A << 4)));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x21);

    e = Execute(TestOp(MOVW_RK, REG_B, 0x34, 0x12));
    EXPECT_EQ(e.next.readw_register(REG_B), 0x1234);

    e = Execute(TestOp(MOVW_RR, REG_C | (REG_B << 4)));
    EXPECT_EQ(e.next.readw_register(REG_C), 0x1234);

    e = Execute(TestOp(MOVL_RK, REG_C, 0x44, 0x55, 0x66, 0x77));
    EXPECT_EQ(e.next.readl_register(REG_C), 0x77665544);

    e = Execute(TestOp(MOVL_RR, REG_D | (REG_C << 4)));
    EXPECT_EQ(e.next.readl_register(REG_D), 0x77665544);

    e = Execute(TestOp(MOVW_RK, REG_DH, 0x11, 0x22));
    EXPECT_EQ(e.next.readl_register(REG_D), 0x22115544);

    e = Execute(TestOp(MOVB_SK, 0x00, 0x12));
    EXPECT_EQ(e.next.sb, 0x12);

    e = Execute(TestOp(MOVB_RS, REG_B));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x12);

    Execute(TestOp(MOVB_RK, REG_B, 0xab));
    e = Execute(TestOp(MOVB_SR, REG_B << 4));
    EXPECT_EQ(e.next.sb, 0xab);
}

TEST_F(BCpuTest, PSH_POP) {
    Delta e;

    Execute(TestOp(PSHX_K, (TYPE_BYTE << 4), 0x22));
    e = Execute(TestOp(POPX_X, (TYPE_BYTE << 4)));
    EXPECT_EQ(e.next.sp, 0x1000);

    Execute(TestOp(PSHX_K, (TYPE_BYTE << 4), 0x23));
    Execute(TestOp(PSHX_K, (TYPE_BYTE << 4), 0x45));
    Execute(TestOp(PSHX_K, (TYPE_BYTE << 4), 0x67));
    e = Execute(TestOp(PSHX_K, (TYPE_BYTE << 4), 0x89));
    EXPECT_EQ(e.next.sp, 0x0FFC);
    e = Execute(TestOp(POPX_R, (TYPE_BYTE << 4) | REG_B));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x89);
    e = Execute(TestOp(POPX_R, (TYPE_BYTE << 4) | REG_B));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x67);
    e = Execute(TestOp(POPX_R, (TYPE_BYTE << 4) | REG_B));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x45);
    e = Execute(TestOp(POPX_R, (TYPE_BYTE << 4) | REG_B));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x23);
    EXPECT_EQ(e.next.sp, 0x1000);


    Execute(TestOp(ANSB_K, 0x00));
    Execute(TestOp(ORSB_K, 0x58));
    e = Execute(TestOp(PSHX_S, TYPE_BYTE << 4));
    EXPECT_EQ(e.wb_value, 0x58);
    EXPECT_EQ(e.next.sp, 0x0FFF);
    Execute(TestOp(ANSB_K, 0x00));
    e = Execute(TestOp(POPX_S, TYPE_BYTE << 4));
    EXPECT_EQ(e.next.sb, 0x58);
    EXPECT_EQ(e.next.sp, 0x1000);

    Execute(TestOp(MOVB_RK, REG_B, 0x12));
    Execute(TestOp(PSHX_R, REG_B | (TYPE_BYTE << 4)));
    Execute(TestOp(MOVB_RK, REG_B, 0x45));
    Execute(TestOp(PSHX_R, REG_B | (TYPE_BYTE << 4)));
    e = Execute(TestOp(POPX_R, REG_C | (TYPE_BYTE << 4)));
    EXPECT_EQ(e.next.readb_register(REG_C), 0x45);
    e = Execute(TestOp(POPX_R, REG_C | (TYPE_BYTE << 4)));
    EXPECT_EQ(e.next.readb_register(REG_C), 0x12);
    EXPECT_EQ(e.next.sp, 0x1000);

    e = Execute(TestOp(PSHX_K, (TYPE_WORD << 4), 0x45, 0x23));
    EXPECT_EQ(e.wb_type, TYPE_WORD);
    EXPECT_EQ(e.wb_value, 0x2345);
    e = Execute(TestOp(POPX_X, (TYPE_WORD << 4)));
    EXPECT_EQ(e.next.sp, 0x1000);

    Execute(TestOp(PSHX_K, (TYPE_WORD << 4), 0x12, 0x34));
    e = Execute(TestOp(POPX_R, (TYPE_WORD << 4) | REG_B));
    EXPECT_EQ(e.next.readw_register(REG_B), 0x3412);
    EXPECT_EQ(e.next.sp, 0x1000);

    Execute(TestOp(MOVL_RK, REG_B, 0x11, 0x22, 0x33, 0x45));
    Execute(TestOp(PSHX_R, (TYPE_LONG << 4) | REG_B));
    e = Execute(TestOp(POPX_R, (TYPE_LONG << 4) | REG_C));
    EXPECT_EQ(e.next.readl_register(REG_C), 0x45332211);
}

TEST_F(BCpuTest, SWP) {
    Delta e;
    Execute(TestOp(MOVB_RK, REG_B, 0x67));
    Execute(TestOp(MOVB_RK, REG_C, 0xab));
    e = Execute(TestOp(SWPB, REG_B | (REG_C << 4)));
    EXPECT_EQ(e.next.readb_register(REG_B), 0xab);
    EXPECT_EQ(e.next.readb_register(REG_C), 0x67);

    Execute(TestOp(MOVW_RK, REG_BH, 0x96, 0x89));
    Execute(TestOp(MOVW_RK, REG_CH, 0xab, 0xcd));
    e = Execute(TestOp(SWPW, REG_BH | (REG_CH << 4)));
    EXPECT_EQ(e.next.readw_register(REG_BH), 0xcdab);
    EXPECT_EQ(e.next.readw_register(REG_CH), 0x8996);

    Execute(TestOp(MOVL_RK, REG_A, 0x12, 0x34, 0x56, 0x78));
    Execute(TestOp(MOVL_RK, REG_D, 0xab, 0xcd, 0xef, 0xbb));
    e = Execute(TestOp(SWPL, REG_A | (REG_D << 4)));
    EXPECT_EQ(e.next.readl_register(REG_D), 0x78563412);
    EXPECT_EQ(e.next.readl_register(REG_A), 0xbbefcdab);
}

TEST_F(BCpuTest, JMP) {
    Delta e;

    // 0xFF padding to make sure it doesn't read too many bytes
    e = ExecuteJump(TestOp(AJMP, 0x99, 0x20, 0xFF));
    EXPECT_EQ(e.next.pc, 0x2099);
    EXPECT_EQ(e.next.sp, 0x1000);

    e = ExecuteJump(TestOp(LAJMP, 0x34, 0x20, 0x01, 0x00, 0xFF));
    EXPECT_EQ(e.next.pc, 0x00012034);
    EXPECT_EQ(e.next.sp, 0x1000);

    e = ExecuteJump(TestOp(RJMP, 0x00, 0x80, 0xFF));
    EXPECT_EQ(e.next.pc, 0xa037);
    EXPECT_EQ(e.next.sp, 0x1000);

    e = ExecuteJump(TestOp(RJMP, 0x00, 0x60, 0xFF));
    EXPECT_EQ(e.next.pc, 0x1003a);
    EXPECT_EQ(e.next.sp, 0x1000);

    e = ExecuteJump(TestOp(LRJMP, 0x00, 0xEF, 0xFF, 0xFF, 0xFF));
    EXPECT_EQ(e.next.pc, 0xef3f);
    EXPECT_EQ(e.next.sp, 0x1000);
}

TEST_F(BCpuTest, JSR) {
    Delta e;

    // 0xFF padding to make sure it doesn't read too many bytes
    e = ExecuteJump(TestOp(AJSR, 0x99, 0x20, 0xFF));
    EXPECT_EQ(e.next.pc, 0x2099);
    EXPECT_EQ(e.next.sp, 0x0ffc);

    e = ExecuteJump(TestOp(LAJSR, 0x34, 0x20, 0x01, 0x00, 0xFF));
    EXPECT_EQ(e.next.pc, 0x00012034);
    EXPECT_EQ(e.next.sp, 0x0ff8);

    e = ExecuteJump(TestOp(RJSR, 0x00, 0x80, 0xFF));
    EXPECT_EQ(e.next.pc, 0xa037);
    EXPECT_EQ(e.next.sp, 0x0ff4);

    e = ExecuteJump(TestOp(RJSR, 0x00, 0x60, 0xFF));
    EXPECT_EQ(e.next.pc, 0x1003a);
    EXPECT_EQ(e.next.sp, 0x0ff0);

    e = ExecuteJump(TestOp(LRJSR, 0x00, 0xEF, 0xFF, 0xFF, 0xFF));
    EXPECT_EQ(e.next.pc, 0xef3f);
    EXPECT_EQ(e.next.sp, 0x0fec);

    e = ExecuteJump(TestOp(RET));
    EXPECT_EQ(e.next.pc, 0x0001003f);
    EXPECT_EQ(e.next.sp, 0x0ff0);

    e = ExecuteJump(TestOp(RET));
    EXPECT_EQ(e.next.pc, 0xa03a);
    EXPECT_EQ(e.next.sp, 0x0ff4);

    e = ExecuteJump(TestOp(RET));
    EXPECT_EQ(e.next.pc, 0x00012037);
    EXPECT_EQ(e.next.sp, 0x0ff8);

    e = ExecuteJump(TestOp(RET));
    EXPECT_EQ(e.next.pc, 0x209e);
    EXPECT_EQ(e.next.sp, 0x0ffc);

    e = ExecuteJump(TestOp(RET));
    EXPECT_EQ(e.next.pc, 0x1003);
    EXPECT_EQ(e.next.sp, 0x1000);
}

TEST_F(BCpuTest, CJMP) {
    Delta e;

    e = ExecuteJump(TestOp(JCC, 0x10, 0x00));
    EXPECT_EQ(e.next.pc, 0x1013);
    EXPECT_EQ(e.next.sp, 0x1000);

    e = ExecuteJump(TestOp(JCS, 0x10, 0x00));
    EXPECT_EQ(e.next.pc, 0x1016);
    EXPECT_EQ(e.next.sp, 0x1000);

    ExecuteJump(TestOp(ORSB_K, 0x01));
    e = ExecuteJump(TestOp(JCS, 0x10, 0x00));
    EXPECT_EQ(e.next.pc, 0x102b);
    EXPECT_EQ(e.next.sp, 0x1000);

    e = ExecuteJump(TestOp(JSS, 0x10, 0x00));
    EXPECT_EQ(e.next.pc, 0x102e);
    EXPECT_EQ(e.next.sp, 0x1000);

    ExecuteJump(TestOp(ORSB_K, 0x80));
    e = ExecuteJump(TestOp(JSS, 0xEF, 0xFF));
    EXPECT_EQ(e.next.pc, 0x1022);
    EXPECT_EQ(e.next.sp, 0x1000);
}

TEST_F(BCpuTest, ADD) {
    Delta e;

    e = Execute(TestOp(ADDB_RK, REG_A, 0x21));
    EXPECT_EQ(e.next.registers[0], 0x21);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(ADDB_RK, REG_A, 0x5F));
    EXPECT_EQ(e.next.registers[0], 0x80);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);

    e = Execute(TestOp(ADDB_RK, REG_A, 0x80));
    EXPECT_EQ(e.next.registers[REG_A], 0x00);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(ADDW_RK, REG_A, 0x11, 0x11));
    EXPECT_EQ(e.next.registers[0], 0x1111);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(ADDB_RK, REG_D, 0x01));

    e = Execute(TestOp(ADDB_RR, REG_A | (REG_D << 4)));
    EXPECT_EQ(e.next.registers[0], 0x1112);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(ADDB_RR, REG_AH | (REG_D << 4)));
    EXPECT_EQ(e.next.registers[0], 0x1212);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(MOVB_RK, REG_D, 0xEE));
    EXPECT_EQ(e.next.registers[REG_D], 0x00EE);

    e = Execute(TestOp(ADDB_RR, REG_AH | (REG_D << 4)));
    EXPECT_EQ(e.next.registers[0], 0x0012);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(ADDB_RR, REG_A | (REG_D << 4)));
    EXPECT_EQ(e.next.registers[0], 0x0000);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(ADDL_RK, REG_B, 0x78, 0x56, 0x34, 0x12));
    EXPECT_EQ(e.next.registers[1], 0x12345678);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(ADDL_RK, REG_C, 0x78, 0x56, 0x34, 0x12));
    EXPECT_EQ(e.next.registers[REG_C], 0x12345678);

    e = Execute(TestOp(ADDL_RR, REG_B | (REG_C << 4)));
    EXPECT_EQ(e.next.registers[REG_B], 0x2468acf0);

    e = Execute(TestOp(ADDL_RR, REG_B | (REG_B << 4)));

    e = Execute(TestOp(ADDL_RR, REG_B | (REG_B << 4)));
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);

    e = Execute(TestOp(ADDL_RR, REG_B | (REG_B << 4)));
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
}

TEST_F(BCpuTest, ADC) {
    Delta e;

    e = Execute(TestOp(ADCB_RK, REG_B, 0x21));
    EXPECT_EQ(e.next.registers[REG_B], 0x21);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(ADCB_RK, REG_B, 0x21));
    EXPECT_EQ(e.next.registers[REG_B], 0x42);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(ADCB_RK, REG_B, 0xFF));
    EXPECT_EQ(e.next.registers[REG_B], 0x41);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(ORSB_K, 0x01)); // SET CARRY
    e = Execute(TestOp(ADCB_RK, REG_B, 0x00));
    EXPECT_EQ(e.next.registers[REG_B], 0x42);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(ADCW_RK, REG_B, 0xFF, 0x00));
    EXPECT_EQ(e.next.registers[1], 0x141);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);

    e = Execute(TestOp(ORSB_K, 0x01)); // SET CARRY
    e = Execute(TestOp(ADCW_RK, REG_B, 0x10, 0x00));
    EXPECT_EQ(e.next.registers[1], 0x152);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);

    e = Execute(TestOp(MOVB_RK, REG_A, 0x77));
    e = Execute(TestOp(ORSB_K, 0x01)); // SET CARRY
    e = Execute(TestOp(ADCW_RR, REG_B | (REG_A << 4)));
    EXPECT_EQ(e.next.registers[REG_B], 0x1CA);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);

    e = Execute(TestOp(ORSB_K, 0x01)); // SET CARRY
    e = Execute(TestOp(ADCL_RK, REG_B, 0xFF, 0xFF, 0xFF, 0xFF));
    EXPECT_EQ(e.next.registers[1], 0x1CA);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
}

TEST_F(BCpuTest, SUB) {
    Delta e;
    Execute(TestOp(MOVB_RK, REG_B, 0x0A));
    e = Execute(TestOp(SUBB_RK, REG_B, 0x08));
    EXPECT_FALSE(e.next.read_flag(FLAG_Z));
    EXPECT_FALSE(e.next.read_flag(FLAG_C));
    EXPECT_FALSE(e.next.read_flag(FLAG_V));
    EXPECT_FALSE(e.next.read_flag(FLAG_S));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x02);

    e = Execute(TestOp(SUBB_RK, REG_B, 0x02));
    EXPECT_FALSE(e.next.read_flag(FLAG_C));
    EXPECT_FALSE(e.next.read_flag(FLAG_V));
    EXPECT_FALSE(e.next.read_flag(FLAG_S));
    EXPECT_TRUE(e.next.read_flag(FLAG_Z));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x00);

    e = Execute(TestOp(SUBB_RK, REG_B, 0x20));
    EXPECT_FALSE(e.next.read_flag(FLAG_Z));
    EXPECT_FALSE(e.next.read_flag(FLAG_V));
    EXPECT_TRUE(e.next.read_flag(FLAG_C));
    EXPECT_TRUE(e.next.read_flag(FLAG_S));
    EXPECT_EQ(e.next.readb_register(REG_B), 0xE0);

    e = Execute(TestOp(SUBW_RK, REG_B, 0x00, 0x01));
    EXPECT_FALSE(e.next.read_flag(FLAG_Z));
    EXPECT_FALSE(e.next.read_flag(FLAG_V));
    EXPECT_TRUE(e.next.read_flag(FLAG_C));
    EXPECT_TRUE(e.next.read_flag(FLAG_S));
    EXPECT_EQ(e.next.readw_register(REG_B), 0xFFE0);

    e = Execute(TestOp(SUBL_RK, REG_B, 0x00, 0x00, 0x21, 0x00));
    EXPECT_FALSE(e.next.read_flag(FLAG_Z));
    EXPECT_FALSE(e.next.read_flag(FLAG_V));
    EXPECT_TRUE(e.next.read_flag(FLAG_C));
    EXPECT_TRUE(e.next.read_flag(FLAG_S));
    EXPECT_EQ(e.next.readl_register(REG_B), 0xFFDFFFE0);

    Execute(TestOp(MOVW_RK, REG_C, 0xE0, 0xFF));
    Execute(TestOp(ANSB_K, 0x00));
    e = Execute(TestOp(SUBL_RR, REG_B | (REG_C << 4)));
    EXPECT_FALSE(e.next.read_flag(FLAG_Z));
    EXPECT_FALSE(e.next.read_flag(FLAG_V));
    EXPECT_FALSE(e.next.read_flag(FLAG_C));
    EXPECT_TRUE(e.next.read_flag(FLAG_S));
    EXPECT_EQ(e.next.readl_register(REG_B), 0xFFDF0000);

    Execute(TestOp(MOVL_RK, REG_C, 0x00, 0x00, 0xFF, 0x7F));
    e = Execute(TestOp(SUBL_RR, REG_B | (REG_C << 4)));
    EXPECT_TRUE(e.next.read_flag(FLAG_V));
    EXPECT_FALSE(e.next.read_flag(FLAG_Z));
    EXPECT_FALSE(e.next.read_flag(FLAG_C));
    EXPECT_FALSE(e.next.read_flag(FLAG_S));
    EXPECT_EQ(e.next.readl_register(REG_B), 0x7FE00000);
}

// same as SUB
TEST_F(BCpuTest, CMP) {
    Delta e;
    Execute(TestOp(MOVB_RK, REG_B, 0x0A));
    e = Execute(TestOp(CMPB_RK, REG_B, 0x08));
    EXPECT_FALSE(e.next.read_flag(FLAG_Z));
    EXPECT_FALSE(e.next.read_flag(FLAG_C));
    EXPECT_FALSE(e.next.read_flag(FLAG_V));
    EXPECT_FALSE(e.next.read_flag(FLAG_S));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x0A);

    Execute(TestOp(MOVB_RK, REG_B, 0x02));
    e = Execute(TestOp(CMPB_RK, REG_B, 0x02));
    EXPECT_FALSE(e.next.read_flag(FLAG_C));
    EXPECT_FALSE(e.next.read_flag(FLAG_V));
    EXPECT_FALSE(e.next.read_flag(FLAG_S));
    EXPECT_TRUE(e.next.read_flag(FLAG_Z));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x02);

    Execute(TestOp(MOVB_RK, REG_B, 0x00));
    e = Execute(TestOp(CMPB_RK, REG_B, 0x20));
    EXPECT_FALSE(e.next.read_flag(FLAG_Z));
    EXPECT_FALSE(e.next.read_flag(FLAG_V));
    EXPECT_TRUE(e.next.read_flag(FLAG_C));
    EXPECT_TRUE(e.next.read_flag(FLAG_S));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x00);

    e = Execute(TestOp(CMPW_RK, REG_B, 0x00, 0x01));
    EXPECT_FALSE(e.next.read_flag(FLAG_Z));
    EXPECT_FALSE(e.next.read_flag(FLAG_V));
    EXPECT_TRUE(e.next.read_flag(FLAG_C));
    EXPECT_TRUE(e.next.read_flag(FLAG_S));
    EXPECT_EQ(e.next.readw_register(REG_B), 0x0000);

    Execute(TestOp(MOVW_RK, REG_B, 0xE0, 0xFF));
    e = Execute(TestOp(CMPL_RK, REG_B, 0x00, 0x00, 0x21, 0x00));
    EXPECT_FALSE(e.next.read_flag(FLAG_Z));
    EXPECT_FALSE(e.next.read_flag(FLAG_V));
    EXPECT_TRUE(e.next.read_flag(FLAG_C));
    EXPECT_TRUE(e.next.read_flag(FLAG_S));
    EXPECT_EQ(e.next.readl_register(REG_B), 0xFFE0);

    Execute(TestOp(MOVL_RK, REG_B, 0xE0, 0xFF, 0xDF, 0xFF));
    Execute(TestOp(MOVW_RK, REG_C, 0xE0, 0xFF));
    Execute(TestOp(ANSB_K, 0x00));
    e = Execute(TestOp(CMPL_RR, REG_B | (REG_C << 4)));
    EXPECT_FALSE(e.next.read_flag(FLAG_Z));
    EXPECT_FALSE(e.next.read_flag(FLAG_V));
    EXPECT_FALSE(e.next.read_flag(FLAG_C));
    EXPECT_TRUE(e.next.read_flag(FLAG_S));

    Execute(TestOp(MOVL_RK, REG_B, 0x00, 0x00, 0xDF, 0xFF));
    Execute(TestOp(MOVL_RK, REG_C, 0x00, 0x00, 0xFF, 0x7F));
    e = Execute(TestOp(CMPL_RR, REG_B | (REG_C << 4)));
    EXPECT_TRUE(e.next.read_flag(FLAG_V));
    EXPECT_FALSE(e.next.read_flag(FLAG_Z));
    EXPECT_FALSE(e.next.read_flag(FLAG_C));
    EXPECT_FALSE(e.next.read_flag(FLAG_S));
}
TEST_F(BCpuTest, AND) {
    Delta e;
    Execute(TestOp(MOVB_RK, REG_B, 0x57));
    e = Execute(TestOp(ANDB_RK, REG_B, 0x25));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x05);

    Execute(TestOp(MOVB_RK, REG_B, 0x35));
    e = Execute(TestOp(ANDB_RK, REG_B, 0x13));
    EXPECT_EQ(e.next.readb_register(REG_B), 0x11);

    Execute(TestOp(MOVW_RK, REG_B, 0x34, 0x12));
    e = Execute(TestOp(ANDW_RK, REG_B, 0x13, 0x30));
    EXPECT_EQ(e.next.readw_register(REG_B), 0x1010);
    EXPECT_FALSE(e.next.read_flag(FLAG_S));

    Execute(TestOp(MOVL_RK, REG_B, 0x45, 0x67, 0xaa, 0xff));
    e = Execute(TestOp(ANDL_RK, REG_B, 0x13, 0x30, 0x80, 0xaa));
    EXPECT_EQ(e.next.readl_register(REG_B), 0xaa802001);
    EXPECT_TRUE(e.next.read_flag(FLAG_S));
    EXPECT_FALSE(e.next.read_flag(FLAG_Z));

    e = Execute(TestOp(ANDL_RR, REG_B | (REG_C << 4)));
    EXPECT_EQ(e.next.readl_register(REG_B), 0x00000000);
    EXPECT_FALSE(e.next.read_flag(FLAG_S));
    EXPECT_TRUE(e.next.read_flag(FLAG_Z));
}

TEST_F(BCpuTest, MUL) {
    Delta e;

    Execute(TestOp(MOVB_RK, REG_B, 0x02));
    e = Execute(TestOp(MULB_RK, REG_B, 0x08));
    EXPECT_EQ(e.next.registers[1], 0x10);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(MULB_RK, REG_B, 0x20));
    EXPECT_EQ(e.next.registers[1], 0x00);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(MOVB_RK, REG_B, 0x03));
    e = Execute(TestOp(MULW_RK, REG_B, 0x64, 0x00));
    EXPECT_EQ(e.next.registers[1], 0x12c);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(MOVL_RK, REG_A, 0xdd, 0xdd, 0xdd, 0x7d));
    e = Execute(TestOp(MULL_RR, REG_B | (REG_A << 4)));
    EXPECT_EQ(e.next.registers[1], 0x7ffffefc);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(MOVB_RK, REG_B, 0x25));
    e = Execute(TestOp(MULB_RK, REG_B, 0x04));
    EXPECT_EQ(e.next.registers[1] & 0xff, 0x94);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);

    e = Execute(TestOp(MULB_RK, REG_B, 0x00));
    EXPECT_EQ(e.next.registers[1] & 0xff, 0x00);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
}

TEST_F(BCpuTest, DIV) {
    uint8_t ops[] = {
        MOVB_RK, 0x01, 0x04,
        DIVB_RK, 0x01, 0x01,
        DIVB_RK, 0x01, 0x02,
        DIVB_RK, 0x01, 0x03,
        MOVB_RK, 0x01, 0x10,
        DIVB_RK, 0x01, 0x05,
        MOVL_RK, 0x00, 0x00, 0x60, 0x01, 0x00,
        MOVL_RK, 0x01, 0x00, 0x00, 0x00, 0x32,
        DIVL_RR, 0x01,
        MOVW_RK, 0x00, 0x00, 0x00,
        DIVW_RR, 0x01,
        MOVB_RK, 0x00, 0x08,
        MOVB_RK, 0x01, 0xFC,
        DIVB_RR, 0x10,
    };

    mem->fill(0x1000, sizeof(ops), ops);
    Delta e;

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x04);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x02);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x00);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x03);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x245d);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0xFFFF);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_T), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);
    cpu->apply(e);

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0] & 0xFF, 0xFE);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_T), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);
    cpu->apply(e);
}

TEST_F(BCpuTest, INC_DEC) {
    uint8_t ops[] = {
        MOVB_RK, 0x01, 0x10,
        INCX, 0x01,
        DECX, 0x01,
        MOVB_RK, 0x01, 0x7F,
        INCX, 0x01,
        DECX, 0x01,
        MOVB_RK, 0x01, 0xFF,
        INCX, 0x01,
        DECX, 0x01,
        MOVW_RK, 0x00, 0xFF, 0xFF,
        INCX, 0x10,
        DECX, 0x10,
        MOVL_RK, 0x02, 0xFF, 0xFF, 0xFF, 0xFF,
        INCX, 0x22,
        DECX, 0x22,
    };
    mem->fill(0x1000, sizeof(ops), ops);
    Delta e;

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x11);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x10);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x80);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x7F);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x00);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0xFF);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);
    cpu->apply(e);

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x0000);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0xFFFF);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);
    cpu->apply(e);

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[2], 0x0000);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[2], 0xFFFFFFFF);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);
    cpu->apply(e);
}

TEST_F(BCpuTest, COM_NEG) {
    uint8_t ops[] = {
        COMX, 0x01,
        NEGX, 0x01,
        MOVB_RK, 0x01, 0x73,
        NEGX, 0x01,
        COMX, 0x01,
        NEGX, 0x11,
        COMX, 0x21,
        MOVB_RK, 0x01, 0xFF,
        COMX, 0x01,
    };
    mem->fill(0x1000, sizeof(ops), ops);
    Delta e;

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0xFF);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x01);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x8D);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x72);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0xFF8E);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0xFFFF0071);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);
    cpu->apply(e);

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.read_flag(FLAG_Z), true);
    cpu->apply(e);
}

TEST_F(BCpuTest, ABS) {
    Delta e;

    e = Execute(TestOp(MOVB_RK, REG_B, 0xF0));
    e = Execute(TestOp(ABSX, REG_B));
    EXPECT_EQ(e.next.registers[1], 0x10);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(ABSX, REG_B));
    EXPECT_EQ(e.next.registers[1], 0x10);

    e = Execute(TestOp(MOVB_RK, REG_B, 0xF0));

    e = Execute(TestOp(ABSX, 0x10 | REG_B)); // ABSW
    EXPECT_EQ(e.next.registers[REG_B], 0xF0);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);

    e = Execute(TestOp(MOVW_RK, REG_B, 0x23, 0x81));
    EXPECT_EQ(e.next.registers[1], 0x8123);

    e = Execute(TestOp(ABSX, 0x10 | REG_B)); // ABSW
    EXPECT_EQ(e.next.registers[1], 0x7EDD);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
}

TEST_F(BCpuTest, Arithmetic) {
    uint8_t ops[] = {
        0x84, 0x00, 0x20, //ADDB A $20
        0x84, 0x00, 0x32, //ADDB A $32
        0x84, 0x00, 0xFF, //ADDB A $FF
        0x94, 0x00, 0x51, //SUBB A $51
        0x85, 0x00, 0x00, 0x01, //ADDW A $100
        0x84, 0x04, 0x01, //ADDB AH $01
        0x84, 0x00, 0x01, //ADDB A $01
        0x94, 0x04, 0x02, //SUBB AH $02
        0x84, 0x00, 0x7F, //ADDB A $7F
    };
    mem->fill(0x1000, sizeof(ops), ops);
    Delta e;

    // test simple add zero flag
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x20);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    cpu->apply(e);

    // test simple add 'adds'
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x52);
    cpu->apply(e);

    // test simple add carry flag
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x51);
    EXPECT_EQ(e.next.read_flag(FLAG_C), true);
    cpu->apply(e);

    // test simple sub, plus flags
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x00);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    cpu->apply(e);

    // test ADDW
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x100);
    EXPECT_EQ(e.next.read_flag(FLAG_S), false);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    cpu->apply(e);

    // test ADDB AH (high byte)
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x200);
    cpu->apply(e);

    // ADDB A $1
    e = cpu->decode();
    cpu->apply(e);

    // SUBB AH $2; test zero flag for high byte (when low byte is non-zero)
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x01);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), false);
    cpu->apply(e);

    // ADDB A $7F; test sign flag, overflow
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x80);
    EXPECT_EQ(e.next.read_flag(FLAG_S), true);
    EXPECT_EQ(e.next.read_flag(FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(FLAG_V), true);
    cpu->apply(e);
}

} // namespace Cpu
} // namespace Bostek
