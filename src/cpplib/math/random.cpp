#include "random.hpp"

#include <math.h>

uint32_t RNG::getIntBelow(uint32_t max) {
    return floor(getFloat() * max);
}

uint32_t RNG::getIntRanged(uint32_t min, uint32_t max) {
    return min + getIntBelow(max - min);
}

float RNG::getFloat() {
    return ((float) getInt()) * (1.0f / (float) UINT32_MAX);
}

float RNG::getUniform(float min, float max) {
    return min + ((max - min) * getFloat());
}

float RNG::getGuassian(float mu, float sigma) {
    float x2pi = getFloat() * M_PI * 2.0f;
    float g2rad = sqrt(-2.0f * log(1.0f - getFloat()));
    float z = cos(x2pi) * g2rad;
    return mu + (z * sigma);
}

// Mersenne Twister constants
#define MT_LEN              624
#define MT_GENPARAM         397
#define MT_DEFAULTSEED      5489
#define MT_GENODDMASK       0x9908b0df
#define MT_INIT_CONST       0x6c078965
#define MT_TEMPER_CONST1    0x9d2c5680
#define MT_TEMPER_CONST2    0xefc60000
#define MT_32BITMASK        0xffffffff
#define MT_HIGHBIT          0x80000000
#define MT_LOWBITS          0x7fffffff

MerseneTwisterRNG::MerseneTwisterRNG() {
    _MT = new uint32_t[MT_LEN];
    _index = MT_LEN + 1; // fresh MT values need to be generated
    _genmask[0] = 0;
    _genmask[1] = MT_GENODDMASK;
    _seed = MT_DEFAULTSEED;
}

MerseneTwisterRNG::MerseneTwisterRNG(uint32_t seed) {
    _MT = new uint32_t[MT_LEN];
    _index = MT_LEN + 1; // fresh MT values need to be generated
    _genmask[0] = 0;
    _genmask[1] = MT_GENODDMASK;
    _seed = seed;
}

MerseneTwisterRNG::~MerseneTwisterRNG() {
    delete _MT;
}

void MerseneTwisterRNG::seed(uint32_t seed) {
    _MT[0] = seed;
    for(int i = 1; i < MT_LEN; i++) {
        _MT[i] = (MT_INIT_CONST * (_MT[i-1] ^ ((_MT[i-1] >> 30) + i))) & MT_32BITMASK;
    }
    _seed = seed;
}

void MerseneTwisterRNG::generate_new_values() {
    uint32_t y;
    int i;
    for(i = 0; i < MT_LEN - MT_GENPARAM; i++) {
        y = (_MT[i] & MT_HIGHBIT) | (_MT[i+1] & MT_LOWBITS);
        _MT[i] = _MT[i + MT_GENPARAM] ^ (y >> 1) ^ (_genmask[y & 0x1]);
    }

    for(; i < MT_LEN - 1; i++) {
        y = (_MT[i] & MT_HIGHBIT) | (_MT[i+1] & MT_LOWBITS);
        _MT[i] = _MT[i + MT_GENPARAM - MT_LEN] ^ (y >> 1) ^ (_genmask[y & 0x1]);
    }

    y = (_MT[MT_LEN - 1] & MT_HIGHBIT) | (_MT[0] & MT_LOWBITS);
    _MT[MT_LEN - 1] = _MT[MT_GENPARAM - 1] ^ (y >> 1) ^ (_genmask[y & 0x1]);
}

uint32_t MerseneTwisterRNG::getInt() {
    if(_index >= MT_LEN) {
        if(_index > MT_LEN) {
            seed(MT_DEFAULTSEED);
        }
        generate_new_values();
        _index = 0;
    }

    uint32_t y = _MT[_index++];
    y ^= (y >> 11);
    y ^= (y << 7) & MT_TEMPER_CONST1;
    y ^= (y << 15) & MT_TEMPER_CONST2;
    y ^= (y >> 18);
    return y;
}

static MerseneTwisterRNG globalRNG;

void RandomSeed(uint32_t seed) {
    globalRNG.seed(seed);
}

uint32_t RandomInt() {
    return globalRNG.getInt();
}

uint32_t RandomIntBelow(uint32_t max) {
    return globalRNG.getIntBelow(max);
}

uint32_t RandomIntRanged(uint32_t min, uint32_t max) {
    return globalRNG.getIntRanged(min, max);
}

float RandomFloat() {
    return globalRNG.getFloat();
}

float RandomUniform(float min, float max) {
    return globalRNG.getUniform(min, max);
}

float RandomGuassian(float mu, float sigma) {
    return globalRNG.getGuassian(mu, sigma);
}
