#ifndef _DRAW_IMAGE_HPP
#define _DRAW_IMAGE_HPP

#include <stdint.h>

struct Pixel {
    public:
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    bool operator==(const Pixel &o) const;
    Pixel(){}
};

class Image {
    int w,h;
    Pixel *pixels;

    public:
    Image(int x, int y);
    ~Image();

    Pixel *getPixelPtr();
    const Pixel *const getPixelPtr() const;
    Pixel getPixel(int x, int y) const;
    //Pixel getFPixel(float x, float y) const;
    void setPixel(int x, int y, const Pixel &p);

    int getWidth() const;
    int getHeight() const;
};

#endif
