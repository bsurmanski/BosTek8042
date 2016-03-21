#include "cpu.hpp"

Cpu::Cpu() : nbr(NULL) {
}

Cpu::~Cpu() {
}

void Cpu::setNorthBridge(NorthBridge *_nbr) {
    nbr = _nbr;
}

void Cpu::clk() {
}

void Cpu::irq(uint8_t ivec) {
}

void Cpu::nmi(uint8_t ivec) {
}
