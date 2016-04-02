#include "image.hpp"

Pixel::Pixel(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {
}

bool Pixel::operator==(const Pixel &o) const {
    return r == o.r && g == o.g && b == o.b && a == o.a;
}

Image::Image(int x, int y) : w(x), h(y) {
    pixels = new Pixel[x * y];
}

Image::~Image() {
    delete[] pixels;
}

Pixel *Image::getPixelPtr() {
    return pixels;
}

const Pixel *const Image::getPixelPtr() const {
    return pixels;
}

Pixel Image::getPixel(int x, int y) const {
    return pixels[w * y + x];
}

/*
Pixel Image::getFPixel(float x, float y) const {
    float fi = fmodf(x * (w-1), NULL);
    float fj = fmodf(y * (h-1), NULL);
    int i = floorf(x * (w-1.000025)); 
    int j = floorf(y * (h-1.000025)); 

    Pixel tl = getPixel(i, j);
    Pixel tr = getPixel((i+1)%w, j);
    int r1= tl.r * (1.0f - fi) + tr.r * fi;
    int g1= tl.g * (1.0f - fi) + tr.g * fi;
    int b1= tl.b * (1.0f - fi) + tr.b * fi;
    int a1= tl.a * (1.0f - fi) + tr.a * fi;

    Pixel bl = getPixel(i, (j+1)%h);
    Pixel br = getPixel((i+1)%w, (j+1)%h);
    int r2= bl.r * (1.0f - fi) + br.r * fi;
    int g2= bl.g * (1.0f - fi) + br.g * fi;
    int b2= bl.b * (1.0f - fi) + br.b * fi;
    int a2= bl.a * (1.0f - fi) + br.a * fi;

    return Pixel(uint8_t((r1 * (1.0f - fj) + r2 * fj) / 4.0f),
                uint8_t((g1 * (1.0f - fj) + g2 * fj) / 4.0f),
                uint8_t((b1 * (1.0f - fj) + b2 * fj) / 4.0f),
                uint8_t((a1 * (1.0f - fj) + a2 * fj) / 4.0f));
}
*/

void Image::setPixel(int x, int y, const Pixel &p) {
    pixels[w * y + x] = p;
}

int Image::getWidth() const {
    return w;
}

int Image::getHeight() const {
    return h;
}
