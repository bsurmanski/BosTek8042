#include <gtest/gtest.h>

#include "cpu.hpp"
#include "memory.hpp"
#include "northBridge.hpp"

class CpuTest : public testing::Test {
    protected:
    NorthBridge *nbr;
    Memory *mem;
    Cpu *cpu;

    virtual void SetUp() {
        mem = new Memory(0xFFFF);
        cpu = new Cpu();
        nbr = new NorthBridge();
        nbr->attachCpu(cpu);
        nbr->attachMemory(mem);

        cpu->pc = 0x0000;
        cpu->sp = 0x1000; // grows down
        cpu->write_breg(Cpu::BREG_ACC, 0);
    }

    virtual void TearDown() {
        delete nbr;
    }
};

TEST_F(CpuTest, PushPull) {
    cpu->pushb(5);
    ASSERT_EQ(cpu->pullb(), 5);

    cpu->pushb(10);
    cpu->pushb(11);
    cpu->pushb(255);
    EXPECT_EQ(cpu->pullb(), 255);
    EXPECT_EQ(cpu->pullb(), 11);
    EXPECT_EQ(cpu->pullb(), 10);

    cpu->pushw(256);
    EXPECT_EQ(cpu->pullw(), 256);

    cpu->pushw(512);
    cpu->pushw(999);
    cpu->pushw(20000);
    EXPECT_EQ(cpu->pullw(), 20000);
    EXPECT_EQ(cpu->pullw(), 999);
    EXPECT_EQ(cpu->pullw(), 512);
}

TEST_F(CpuTest, Swap8) {
    uint8_t ops[] = {
        0x09, // SWA H
        0x0A, // SWA L
        0x0B, // SWA B
        0x0C, // SWA C
        0x0D, // SWA D
        0x0E, // SWA E
        0x0F, // SWA M
    };
    mem->fill(0x0000, sizeof(ops), ops);

    cpu->write_breg(Cpu::BREG_ACC, 0x0F);

    // SWA H
    cpu->write_breg(Cpu::BREG_H, 0x32);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x32);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_H), 0x0F);

    // SWA L
    cpu->write_breg(Cpu::BREG_L, 0x13);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x13);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_L), 0x32);

    // SWA B
    cpu->write_breg(Cpu::BREG_B, 0xFB);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xFB);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_B), 0x13);

    // SWA C
    cpu->write_breg(Cpu::BREG_C, 0xCA);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xCA);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_C), 0xFB);

    // SWA D
    cpu->write_breg(Cpu::BREG_D, 0xDD);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xDD);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_D), 0xCA);

    // SWA E
    cpu->write_breg(Cpu::BREG_E, 0xEF);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xEF);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_E), 0xDD);

    // SWA M
    cpu->write_wreg(Cpu::WREG_HL, 0x0FFF);
    cpu->pushb(0x5A);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x5A);
    EXPECT_EQ(cpu->pullb(), 0xEF);
}

TEST_F(CpuTest, LoadStore8) {
    uint8_t ops[] = {
        0x18, 0x00, 0x00, // STM 0x0000, A
        0x19, 0xFF, 0xFF, // STM 0xFFFF, H
        0x1A, 0x80, 0x80, // STM 0x8080, L
        0x1B, 0x86, 0x80, // STM 0x8086, B
        0x1C, 0x08, 0x80, // STM 0x8008, C
        0x1D, 0x87, 0x80, // STM 0x8087, D
        0x1E, 0x34, 0x12, // STM 0x1234, E
        0x1F, 0x78, 0x56, // STM 0x5678, M

        0x17, 0x00, 0x00, // LDM 0x0000, M
        0x16, 0xFF, 0xFF, // LDM 0xFFFF, E
        0x15, 0x80, 0x80, // LDM 0x8080, D
        0x14, 0x86, 0x80, // LDM 0x8086, C
        0x13, 0x08, 0x80, // LDM 0x8008, B
        0x12, 0x87, 0x80, // LDM 0x8087, L
        0x11, 0x34, 0x12, // LDM 0x1234, H
        0x10, 0x78, 0x56, // LDM 0x5678, A
    };

    mem->fill(0x0000, sizeof(ops), ops);
    cpu->write_breg(Cpu::BREG_ACC, 0x65);
    cpu->write_breg(Cpu::BREG_H, 0x02);
    cpu->write_breg(Cpu::BREG_L, 0x64);
    cpu->write_breg(Cpu::BREG_B, 0x47);
    cpu->write_breg(Cpu::BREG_C, 0x32);
    cpu->write_breg(Cpu::BREG_D, 0xCA);
    cpu->write_breg(Cpu::BREG_E, 0x24);

    // STM A
    cpu->clk();
    EXPECT_EQ(mem->readb(0x0000), 0x65);

    // STM H
    cpu->clk();
    EXPECT_EQ(mem->readb(0xFFFF), 0x02);

    // STM L
    cpu->clk();
    EXPECT_EQ(mem->readb(0x8080), 0x64);

    // STM B
    cpu->clk();
    EXPECT_EQ(mem->readb(0x8086), 0x47);

    // STM C
    cpu->clk();
    EXPECT_EQ(mem->readb(0x8008), 0x32);

    // STM D
    cpu->clk();
    EXPECT_EQ(mem->readb(0x8087), 0xCA);

    // STM E
    cpu->clk();
    EXPECT_EQ(mem->readb(0x1234), 0x24);

    // STM M
    mem->writeb(0x0264, 0x57);
    cpu->clk();
    EXPECT_EQ(mem->readb(0x5678), 0x57);

    // LDM M
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_M), 0x65);
    EXPECT_EQ(mem->readb(0x0000), 0x65);

    // LDM E
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_E), 0x02);

    // LDM D
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_D), 0x64);

    // LDM C
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_C), 0x47);

    // LDM B
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_B), 0x32);

    // LDM L
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_L), 0xCA);

    // LDM H
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_H), 0x24);

    // LDM A
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x57);
}

TEST_F(CpuTest, Transfer8) {
    uint8_t ops[] = {
        0x20, 0x42, // TRA 0x42
        0x21, // TRA H
        0x22, // TRA L
        0x23, // TRA B
        0x24, // TRA C
        0x25, // TRA D
        0x26, // TRA E
        0x27, // TRA M

        0x29, // TAR H
        0x2A, // TAR L
        0x2B, // TAR B
        0x2C, // TAR C
        0x2D, // TAR D
        0x2E, // TAR E
        0x2F, // TAR M
    };
    mem->fill(0x0000, sizeof(ops), ops);

    // TRA K
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x42);

    // TRA H
    cpu->write_breg(Cpu::BREG_H, 0xFF);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xFF);

    // TRA L
    cpu->write_breg(Cpu::BREG_L, 0x00);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x00);

    // TRA B
    cpu->write_breg(Cpu::BREG_B, 0xCA);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xCA);

    // TRA C
    cpu->write_breg(Cpu::BREG_C, 0x41);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x41);

    // TRA D
    cpu->write_breg(Cpu::BREG_D, 0x12);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x12);

    // TRA E
    cpu->write_breg(Cpu::BREG_E, 0x46);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x46);

    // TRA M
    cpu->write_wreg(Cpu::WREG_HL, 0x6700);
    mem->writeb(0x6700, 0x89);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x89);

    // TAR H
    cpu->write_breg(Cpu::BREG_ACC, 0x42);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_H), 0x42);

    // TAR L
    cpu->write_breg(Cpu::BREG_ACC, 0x45);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_L), 0x45);

    // TAR B
    cpu->write_breg(Cpu::BREG_ACC, 0x21);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_B), 0x21);

    // TAR C
    cpu->write_breg(Cpu::BREG_ACC, 0x44);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_C), 0x44);

    // TAR D
    cpu->write_breg(Cpu::BREG_ACC, 0xFA);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_D), 0xFA);

    // TAR E
    cpu->write_breg(Cpu::BREG_ACC, 0xFC);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_E), 0xFC);

    // TAR M
    cpu->write_breg(Cpu::BREG_ACC, 0x6E);
    cpu->write_wreg(Cpu::WREG_HL, 0xFFF0);
    cpu->clk();
    EXPECT_EQ(mem->readw(0xFFF0), 0x6E);
}

TEST_F(CpuTest, PushPull8) {
    uint8_t ops[] = {
        0x38, // PSH A
        0x39, // PSH H
        0x3A, // PSH L
        0x3B, // PSH B
        0x3C, // PSH C
        0x3D, // PSH D
        0x3E, // PSH E
        0x3F, // PSH M

        0x30, // PUL A
        0x31, // PUL H
        0x32, // PUL L
        0x33, // PUL B
        0x34, // PUL C
        0x35, // PUL D
        0x36, // PUL E
        0x37, // PUL M
    };
    mem->fill(0x0000, sizeof(ops), ops);

    cpu->write_breg(Cpu::BREG_ACC, 0x98);
    cpu->write_breg(Cpu::BREG_H, 0xAB);
    cpu->write_breg(Cpu::BREG_L, 0xCB);
    cpu->write_breg(Cpu::BREG_B, 0x12);
    cpu->write_breg(Cpu::BREG_C, 0x32);
    cpu->write_breg(Cpu::BREG_D, 0x44);
    cpu->write_breg(Cpu::BREG_E, 0x51);
    mem->writeb(0xABCB, 0xF3);

    // PSH
    for(int i = 1; i <= 8; i++) {
        cpu->clk();
        EXPECT_EQ(cpu->sp, 0x1000-i);
    }


    mem->writeb(0x5144, 0xFF);
    // PUL
    for(int i = 7; i >= 0; i--) {
        cpu->clk();
        EXPECT_EQ(cpu->sp, 0x1000-i);
    }

    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xF3);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_H), 0x51);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_L), 0x44);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_B), 0x32);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_C), 0x12);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_D), 0xCB);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_E), 0xAB);
    EXPECT_EQ(mem->readb(0x5144), 0x98);
}

TEST_F(CpuTest, Jump) {
    // jump address will stay the same at 0x2000
    // first byte will be modified across tests to change jump type
    uint8_t ops[] = {
        0x68, 0x00, 0x20,
    };

    mem->fill(0x0000, sizeof(ops), ops);

    // jump flag set/clear
    // SZVITFHC
    for(int i = 0; i < 8; i++) {
        // jump flag set
        Cpu::Flag f = (Cpu::Flag) (7-i);
        cpu->set_flag(f, 0);
        mem->writeb(0x0000, Cpu::JSC-i); // JbC
        cpu->clk();
        EXPECT_EQ(cpu->pc, 0x2000); // we jumped

        cpu->pc = 0x0000;
        cpu->set_flag(f, 1);
        cpu->clk();
        EXPECT_EQ(cpu->pc, 0x0003); // we did not jump

        // jump flag set
        cpu->set_flag(f, 0);
        mem->writeb(0x0000, Cpu::JSS-i); // JbS
        cpu->clk();
        EXPECT_EQ(cpu->pc, 0x0003); // we didn't jump

        cpu->pc = 0x0000;
        cpu->set_flag(f, 1);
        cpu->clk();
        EXPECT_EQ(cpu->pc, 0x2000); // we jumped
    }

    cpu->pc = 0x0000;
    mem->writeb(0x0000, Cpu::JMPK); // JMP K
    mem->writew(0x0001, 0x2000);
    cpu->clk();
    EXPECT_EQ(cpu->pc, 0x2000);

    cpu->pc = 0x0000;
    cpu->write_wreg(Cpu::WREG_HL, 0x3000);
    mem->writeb(0x0000, Cpu::JMPM); // JMP M
    cpu->clk();
    EXPECT_EQ(cpu->pc, 0x3000);

}

TEST_F(CpuTest, Arithmetic8) {
    uint8_t ops[] = {
        0x80, 0x02, // ADD 2
        0x80, 0x40, // ADD 0x40
        0x83,       // ADD B
        0x87,       // ADD M

        0x90, 0x05, // SUB 5
        0x90, 0x40, // SUB 0x40
        0x94,       // SUB C
        0x97,       // SUB M

        //TODO: cmp
        0xA8, 0x55, // AND 0x55
        0xAF, // AND M

        0xB7, // ORR M
        0xBF, // XOR M

        0xC0, // SHL
        0xC1, // SHR
        0xC2, // ROL
        0xC3, // ROR
        0xC4, // COM
        0xC5, // NEG
        0xC5, // NEG
    };

    mem->fill(0x0000, sizeof(ops), ops);

    // ADD 0x02
    cpu->clk();
    EXPECT_EQ(cpu->pc, 0x0002);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x02);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_Z), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_S), 0);

    // ADD 0x40
    cpu->clk();
    EXPECT_EQ(cpu->pc, 0x0004);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x42);

    // B = 0x20
    // ADD B
    cpu->write_breg(Cpu::BREG_B, 0x20);
    cpu->clk();
    EXPECT_EQ(cpu->pc, 0x0005);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x62);

    // ADD M
    cpu->write_wreg(Cpu::WREG_HL, 0x0FFF); // 0x0FFF = stack
    cpu->pushb(0x11);
    cpu->clk();
    cpu->pullb();
    EXPECT_EQ(cpu->pc, 0x0006);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x73);

    // SUB 0x05
    cpu->clk();
    EXPECT_EQ(cpu->pc, 0x0008);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x6E);

    // SUB 0x40
    cpu->clk();
    EXPECT_EQ(cpu->pc, 0x000A);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x2E);

    // C = 0x0f
    // SUB C
    cpu->write_breg(Cpu::BREG_C, 0x0F);
    cpu->clk();
    EXPECT_EQ(cpu->pc, 0x000B);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x1F);

    // SUB M
    cpu->write_wreg(Cpu::WREG_HL, 0x0FFF); // 0x0FFF = stack
    cpu->pushb(0x20);
    cpu->clk();
    cpu->pullb();
    EXPECT_EQ(cpu->pc, 0x000C);
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xFF);

    // AND 0x55
    cpu->write_breg(Cpu::BREG_ACC, 0x71);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x51);

    // AND M
    cpu->write_breg(Cpu::BREG_ACC, 0x71);
    cpu->pushb(0x64);
    cpu->clk();
    cpu->pullb();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x60);

    // ORR M
    cpu->write_breg(Cpu::BREG_ACC, 0x25);
    cpu->pushb(0x47);
    cpu->clk();
    cpu->pullb();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x67);

    // XOR M
    cpu->write_breg(Cpu::BREG_ACC, 0x25);
    cpu->pushb(0x31);
    cpu->clk();
    cpu->pullb();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x14);

    // SHL
    cpu->write_breg(Cpu::BREG_ACC, 0x72);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xE4);

    // SHR
    cpu->write_breg(Cpu::BREG_ACC, 0x35);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x1A);

    // ROL
    cpu->write_breg(Cpu::BREG_ACC, 0xA2);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x45);

    // ROR
    cpu->write_breg(Cpu::BREG_ACC, 0x35);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x9A);

    // COM
    cpu->write_breg(Cpu::BREG_ACC, 0x35);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xCA);

    // NEG
    cpu->write_breg(Cpu::BREG_ACC, 0xFF);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x01);

    // NEG
    cpu->write_breg(Cpu::BREG_ACC, 0x36);
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xCA);
}

TEST_F(CpuTest, Arithmetic8_Flags) {
    uint8_t ops[] = {
        0x80, 0xFF, // ADD FF
        0x80, 0x01, // ADD 01
        0x80, 0x7F, // ADD 7F
        0x80, 0x7F, // ADD 7F
        0x90, 0xFF, // SUB -1
        0xC5, // NEG
        0xC5, 0xC4, // NEG, COM
    };

    mem->fill(0x0000, sizeof(ops), ops);

    // test sign
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xFF);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_S), 1);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_Z), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_V), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_C), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_H), 0);

    // test zero, carry, half carry
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x00);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_S), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_Z), 1);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_V), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_C), 1);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_H), 1);

    // test flags clearing
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x7F);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_S), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_Z), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_V), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_C), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_H), 0);

    // test overflow
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xFE);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_S), 1);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_Z), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_V), 1);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_C), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_H), 1);

    // test sub carry
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0xFF);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_S), 1);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_Z), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_V), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_C), 1);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_H), 1);

    // NEG
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x01);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_S), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_Z), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_V), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_C), 1);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_H), 1);

    // NEG, COM
    cpu->clk();
    cpu->clk();
    EXPECT_EQ(cpu->read_breg(Cpu::BREG_ACC), 0x00);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_S), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_Z), 1);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_V), 0);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_C), 1);
    EXPECT_EQ(cpu->get_flag(Cpu::FLAG_H), 1);
}
