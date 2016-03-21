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
    cpu->setNorthBridge(this);
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


uint8_t NorthBridge::readb(uint32_t addr) {
    if(mem) return mem->readb(addr);
    return 0x00;
}

uint16_t NorthBridge::readw(uint32_t addr) {
    if(mem) return mem->readw(addr);
    return 0x0000;
}

uint32_t NorthBridge::readl(uint32_t addr) {
    if(mem) return mem->readl(addr);
    return 0x00000000;
}

void NorthBridge::writeb(uint32_t addr, uint8_t v) {
    if(mem) return mem->writeb(addr, v);
}

void NorthBridge::writew(uint32_t addr, uint16_t v) {
    if(mem) return mem->writew(addr, v);
}

void NorthBridge::writel(uint32_t addr, uint32_t v) {
    if(mem) return mem->writel(addr, v);
}
