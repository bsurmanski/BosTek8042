#include "northBridge.hpp"

#include "cpu.hpp"
#include "memory.hpp"

#include <stddef.h>

NorthBridge::NorthBridge() : cpu(NULL), mem(NULL) {
}

NorthBridge::~NorthBridge() {
    if(cpu) cpu->release();
    if(mem) mem->release();
}

void NorthBridge::attachCpu(Cpu *_cpu) {
    cpu = _cpu;
    cpu->nbr = this;
}

void NorthBridge::attachMemory(Memory *_mem) {
    mem = _mem;
}

void NorthBridge::detachCpu() {
    cpu = NULL;
}

void NorthBridge::detachMemory() {
    mem = NULL;
}


uint8_t NorthBridge::readb(uint16_t addr) {
    if(mem) return mem->readb(addr);
    return 0x00;
}

uint16_t NorthBridge::readw(uint16_t addr) {
    if(mem) return mem->readw(addr);
    return 0x0000;
}

void NorthBridge::writeb(uint16_t addr, uint8_t v) {
    if(mem) return mem->writeb(addr, v);
}

void NorthBridge::writew(uint16_t addr, uint16_t v) {
    if(mem) return mem->writew(addr, v);
}
