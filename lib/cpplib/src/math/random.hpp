#ifndef _MATH_RANDOM_HPP
#define _MATH_RANDOM_HPP

#include <stdint.h>

class RNG {
    public:
    virtual void seed(uint32_t seed) = 0;
    virtual uint32_t getInt() = 0;
    uint32_t getIntBelow(uint32_t max);
    uint32_t getIntRanged(uint32_t min, uint32_t max);
    float getFloat();
    float getUniform(float min, float max);
    float getGuassian(float mu, float sigma);
};

class MerseneTwisterRNG : public RNG {
    uint32_t *_MT;
    int _index;
    uint32_t _genmask[2];
    uint32_t _seed;

    void generate_new_values();
    public:
    MerseneTwisterRNG();
    MerseneTwisterRNG(uint32_t seed);
    virtual ~MerseneTwisterRNG();
    virtual void seed(uint32_t seed);
    virtual uint32_t getInt();
};

void RandomSeed(uint32_t seed);
uint32_t RandomInt();
uint32_t RandomIntBelow(uint32_t max);
uint32_t RandomIntRanged(uint32_t min, uint32_t max);
float RandomFloat();
float RandomUniform(float min, float max);
float RandomGuassian(float mu, float sigma);

#endif
