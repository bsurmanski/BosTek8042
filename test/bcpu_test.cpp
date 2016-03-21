#include <gtest/gtest.h>

#include "../src/bcpu.hpp"
#include "../src/memory.hpp"
#include "../src/northBridge.hpp"

class BCpuTest : public testing::Test {
    public:
    NorthBridge *nbr;
    Memory *mem;
    BCpu *cpu;

    virtual void SetUp() {
        mem = new Memory(0xFFFF);
        cpu = new BCpu(0x1000, 0x2000);
        nbr = new NorthBridge;
        nbr->attachCpu(cpu);
        nbr->attachMemory(mem);
    }

    virtual void TearDown() {
        delete nbr;
    }
};

TEST_F(BCpuTest, ADD) {
    uint8_t ops[] = {
        0x84, 0x00, 0x21, //ADDB A $21
        0x84, 0x00, 0x5F, //ADDB A $5F
        0x84, 0x00, 0x80, //ADDB A $80
        0x85, 0x00, 0x11, 0x11, //ADDW A $1111
        0x84, 0x03, 0x01, //ADDB D $01
        0x80, 0x30, //ADDB A D
        0x80, 0x34, //ADDB AH D
        0x34, 0x03, 0xEE, // MOV D $EE
        0x80, 0x34, //ADDB AH D
        0x80, 0x30, //ADDB A D
        0x86, 0x01, 0x78, 0x56, 0x34, 0x12, //ADDL B $12345678
        0x86, 0x02, 0x78, 0x56, 0x34, 0x12, //ADDL C $12345678
        0x82, 0x21, // ADDL B C
        0x82, 0x11, // ADDL B B
        0x82, 0x11, // ADDL B B
        0x82, 0x11, // ADDL B B
    };
    mem->fill(0x1000, sizeof(ops), ops);
    BCpu::Effect e;

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x21);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x80);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_S), true);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x00);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x1111);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[3], 0x01);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x1112);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x1212);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[3], 0x00EE);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x0012);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x0000);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), true);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x12345678);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_S), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[2], 0x12345678);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.registers[1], 0x2468acf0);
    cpu->apply(e);

    e = cpu->decode();
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), true);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_S), true);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), false);
    cpu->apply(e);

    e = cpu->decode();
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), true);
    cpu->apply(e);
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
    BCpu::Effect e;

    // test simple add zero flag
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x20);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_S), false);
    cpu->apply(e);

    // test simple add 'adds'
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x52);
    cpu->apply(e);

    // test simple add carry flag
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x51);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), true);
    cpu->apply(e);

    // test simple sub, plus flags
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x00);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), false);
    cpu->apply(e);

    // test ADDW
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x100);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_S), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), false);
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
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), true);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), false);
    cpu->apply(e);

    // ADDB A $7F; test sign flag, overflow
    e = cpu->decode();
    EXPECT_EQ(e.next.registers[0], 0x80);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_S), true);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_Z), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_C), false);
    EXPECT_EQ(e.next.read_flag(BCpu::FLAG_V), true);
    cpu->apply(e);
}
