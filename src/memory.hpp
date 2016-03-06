#ifndef _BOSTEK_MEMORY_HPP
#define _BOSTEK_MEMORY_HPP

#include <stdint.h>

#include "object.hpp"

class Memory : public Object {
    int size;
    uint8_t *ptr;

    public:
    Memory(int size);
    ~Memory();

    void zero();
    void fill(uint16_t addr, int n, void *ptr);
    uint8_t readb(uint16_t addr);
    uint16_t readw(uint16_t addr);
    void writeb(uint16_t addr, uint8_t v);
    void writew(uint16_t addr, uint16_t v);
};

#endif
