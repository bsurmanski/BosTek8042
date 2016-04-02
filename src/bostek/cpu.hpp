#ifndef _BOSTEK_CPU_HPP
#define _BOSTEK_CPU_HPP

#include "cpplib/common/object.hpp"
#include "northBridge.hpp"

#include <stdint.h>
#include <stddef.h>


class Cpu : public Object {
    protected:
    NorthBridge *nbr;

    public:
    Cpu();
    virtual ~Cpu();
    void setNorthBridge(NorthBridge *_nbr);
    virtual void clk();
    virtual void irq(uint8_t ivec);
    virtual void nmi(uint8_t ivec);
};

#endif
