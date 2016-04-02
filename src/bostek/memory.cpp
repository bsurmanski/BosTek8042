#include "memory.hpp"

Memory::Memory(int _size) {
    size = _size;
    ptr = new uint8_t[size];
}

Memory::~Memory() {
    delete[] ptr;
}

void Memory::zero() {
    for(int i = 0; i < size; i++) {
        ptr[i] = '\0';
    }
}

void Memory::fill(uint32_t addr, int n, void *ptr) {
    for(int i = 0; i < n; i++) {
        writeb(addr + i, ((uint8_t*)ptr)[i]);
    }
}

uint8_t Memory::readb(uint32_t addr) {
    if(addr >= size) return 0xFF; // TODO: trap?
    return ptr[addr];
}

uint16_t Memory::readw(uint32_t addr) {
    return (readb(addr+1) << 8) | readb(addr);
}

uint32_t Memory::readl(uint32_t addr) {
    return  (readb(addr+3) << 24) | (readb(addr+2) << 16) | (readb(addr+1) << 8) | readb(addr);
}

void Memory::writeb(uint32_t addr, uint8_t v) {
    if(addr >= size) return;
    ptr[addr] = v;
}

void Memory::writew(uint32_t addr, uint16_t v) {
    writeb(addr, v & 0x000000FF);
    writeb(addr+1, (v & 0x0000FF00) >> 8);
}

void Memory::writel(uint32_t addr, uint32_t v) {
    writeb(addr, v & 0x000000FF);
    writeb(addr+1, (v & 0x0000FF00) >> 8);
    writeb(addr+2, (v & 0x00FF0000) >> 16);
    writeb(addr+3, (v & 0xFF000000) >> 24);
}
