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

void Memory::fill(uint16_t addr, int n, void *ptr) {
    for(int i = 0; i < n; i++) {
        writeb(addr + i, ((uint8_t*)ptr)[i]);
    }
}

uint8_t Memory::readb(uint16_t addr) {
    if(addr > size) return 0xFF;
    return ptr[addr];
}

uint16_t Memory::readw(uint16_t addr) {
    if(addr == 0xFFFF || addr+1 > size) return 0xFF;
    return (ptr[addr+1] << 8) | ptr[addr];
}

void Memory::writeb(uint16_t addr, uint8_t v) {
    if(addr > size) return;
    ptr[addr] = v;
}

void Memory::writew(uint16_t addr, uint16_t v) {
    if(addr == 0xFFFF || addr+1 > size) return;
    ptr[addr] = v & 0x00FF;
    ptr[addr+1] = (v & 0xFF00) >> 8;
}

