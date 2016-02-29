#include "float16.hpp"

float16_t::float16_t() {
    v = 0x0000; // zero value
}

float16_t::float16_t(const float16_t &o) {
    v = o.v;
}

uint16_t float16_t::packed() const {
    return v;
}

float16_t float16_t::unpack(uint16_t _v) {
    float16_t ret;
    ret.v = _v;
    return ret;
}

uint8_t float16_t::sign() const {
    return (v >> 15) & 0x0001;
}

uint8_t float16_t::exponent() const {
    return (v >> 10) & 0x001F;
}

int8_t float16_t::exponent_unbiased() const {
    return exponent() - 15;
}

uint16_t float16_t::significant() const {
    return v & 0x03FF;
}

void float16_t::set_sign(uint8_t s) {
    v = (v & 0x7FFF) | (((uint16_t)s) << 15);
}

void float16_t::set_exponent(uint8_t e) {
    v = (v & 0x83FF) | ((((uint16_t)e) << 10) & 0x7C00);
}

void float16_t::set_exponent_unbiased(int8_t e) {
    set_exponent((uint8_t) (e+15));
}
void float16_t::set_significant(uint16_t s) {
    v = (v & 0xFC00) | (s & 0x03FF);
}

float16_t float16_t::add(const float16_t &o) const {

}

float16_t float16_t::mul(const float16_t &o) const {
    float16_t ret;
    uint16_t eadd = exponent() + o.exponent();
    uint32_t smul = significant() * o.significant();
    //TODO
}

float16_t float16_t::sub(const float16_t &o) const {

}

float16_t float16_t::div(const float16_t &o) const {

}

float16_t float16_t::mod(const float16_t &o) const {

}

float16_t float16_t::max(const float16_t &o) const {
    if(exponent() == o.exponent()) {
        if(significant() > o.significant()) {
            return *this;
        }
        return o;
    }

    if(exponent() > o.exponent()) {
        return *this;
    }
    return o;
}

float16_t float16_t::min(const float16_t &o) const {
    if(exponent() == o.exponent()) {
        if(significant() < o.significant()) {
            return *this;
        }
        return o;
    }

    if(exponent() < o.exponent()) {
        return *this;
    }
    return o;
}

float16_t float16_t::srt() const {

}

float16_t float16_t::sin() const {

}

float16_t float16_t::cos() const {

}

float16_t float16_t::tan() const {

}

float16_t float16_t::abs() const {
    float16_t f(*this);
    f.set_sign(0); // set sign to zero
    return f;
}

float16_t float16_t::neg() const {
    float16_t f(*this);
    f.set_sign(!sign()); // set sign to inverse of current sign
    return f;
}
