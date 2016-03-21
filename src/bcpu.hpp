#ifndef _BOSTEK_BCPU_HPP
#define _BOSTEK_BCPU_HPP

#include "cpu.hpp"

class BCpu : public Cpu {
    enum Type {
        BYTE=0,
        WORD=1,
        LONG=2,
        FLOAT=3,
        NONE=4,
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

    uint32_t sign_mask(Type ty);
    uint32_t type_mask(Type ty);
    bool is_negative(Type ty, uint32_t val);
    uint32_t zxt_value(Type ty, uint32_t val);
    uint32_t sxt_value(Type ty, uint32_t val);
    uint32_t neg_value(Type ty, uint32_t val);
    uint32_t abs_value(Type ty, uint32_t val);
    uint64_t pow_value(uint32_t v1, uint32_t v2); // assumes unsigned

    struct State {
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

        State();
    };

    struct Effect {
        State next;

        Type wb_type;
        uint32_t wb_addr;
        uint32_t wb_value;

        Effect();
        Effect(State s);
        Effect(State s, Type ty, uint32_t addr, uint32_t v);
    };

    State state;
    Effect nextEffect;

    int op_wait; // how many clks to wait until commiting

    Effect decode_control(uint8_t op1);
    Effect decode_move(uint8_t op1);
    Effect decode_jump(uint8_t op1);
    Effect decode_arithmetic(uint8_t op1);
    Effect decode();
    void apply(Effect e);

    public:
    BCpu();
    BCpu(uint32_t pc, uint32_t sp);
    virtual void clk();

#ifndef FRIEND_TEST
#define FRIEND_TEST(X,Y)
#endif
    friend class BCpuTest;
    FRIEND_TEST(BCpuTest, Arithmetic);
    FRIEND_TEST(BCpuTest, ADD);
    FRIEND_TEST(BCpuTest, ADC);
};

#endif
