#ifndef _BOSTEK_FLOAT16_HPP
#define _BOSTEK_FLOAT16_HPP

#include <stdint.h>

struct float16_t {
    // 1 bit sign
    // 5 bit exponent (bias -15)
    // 10 bit significant

    // 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
    // |s e- e- e- e- e- g- g- g- g- g- g- g- g- g- g|
    uint16_t v;

    public:
    float16_t();
    float16_t(const float16_t &o);

    uint16_t packed() const;
    static float16_t unpack(uint16_t);

    uint8_t sign() const;
    uint8_t exponent() const;
    int8_t exponent_unbiased() const;
    uint16_t significant() const;

    void set_sign(uint8_t s); // sign must be in bit 0
    void set_exponent(uint8_t e);
    void set_exponent_unbiased(int8_t e);
    void set_significant(uint16_t s);

    float16_t add(const float16_t &o) const;
    float16_t mul(const float16_t &o) const;
    float16_t sub(const float16_t &o) const;
    float16_t div(const float16_t &o) const;
    float16_t mod(const float16_t &o) const;

    float16_t max(const float16_t &o) const;
    float16_t min(const float16_t &o) const;

    float16_t srt() const;
    float16_t irt() const;
    float16_t sin() const;
    float16_t cos() const;
    float16_t tan() const;
    float16_t abs() const;
    float16_t neg() const;
};

#endif
