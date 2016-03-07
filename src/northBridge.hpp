#ifndef _BOSTEK_NORTH_BRIDGE_HPP
#define _BOSTEK_NORTH_BRIDGE_HPP

#include <stdint.h>
#include "object.hpp"

class Cpu;
class Memory;

/**
 * Links together Cpu/Memory/IO
 *
 * Redirects memory mapped registers to appropriate location
 */
class NorthBridge : public Object {
    Cpu *cpu;
    Memory *mem;

    public:
    NorthBridge();
    ~NorthBridge();

    void attachCpu(Cpu *cpu);
    void attachMemory(Memory *mem);
    void detachCpu();
    void detachMemory();

    uint8_t readb(uint16_t addr);
    uint16_t readw(uint16_t addr);
    void writeb(uint16_t addr, uint8_t v);
    void writew(uint16_t addr, uint16_t v);
};

#endif
