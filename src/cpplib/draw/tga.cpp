#include "tga.hpp"

#define RLE_REPEAT_MASK 0x7F
#define RLE_FLAG_MASK 0x80

struct TGAHeader {
    uint8_t id_len;
    uint8_t cmap_type; // 0 if no cmap, 1 if cmap
    uint8_t img_type;

    //Colormap Specification
    struct HeaderCmap {
        uint16_t offset;
        uint16_t len;
        uint8_t entry_sz;
    } cmap;

    //Image specification
    struct HeaderImg {
        uint16_t xorg;
        uint16_t yorg; //TODO: properly read bottom left origined image
        uint16_t w;
        uint16_t h;
        uint8_t depth;
        uint8_t discriptor;
    } img;

    bool isValid() {
        if (cmap_type != 0 && cmap_type != 1) {
            return false;
        }

        return true; 
    }

    bool hasCmap() {
        return cmap_type;
    }

    bool hasRLE() {
        return img_type >= 9 && img_type <= 11;
    }

    int bytesPerPixel() {
        return img.depth / 8;
    }
};

void formatPixel(Pixel &p) {
    // swap R and B pixels, because TGA's store in BGR (or BGRA) format
    int tmp = p.r;
    p.r = p.b;
    p.b = tmp;
}

#include <stdio.h>

Image *loadTga(Input &file) {
    TGAHeader header;
    file.read(&header.id_len, 1, 1);
    file.read(&header.cmap_type, 1, 1);
    file.read(&header.img_type, 1, 1);
    file.read(&header.cmap, 5, 1);
    file.read(&header.img, 10, 1);

    if(file.eof() || !header.isValid()) {
        return NULL; // file too short
    }

    if(header.hasCmap()) {
        return NULL; // cmap not supported
    }

    Image *image = new Image(header.img.w, header.img.h);
    
    Pixel pixel(0x00, 0x00, 0x00, 0xff);
    bool repeat = false; // whether to repeat the same pixel, or skip looking for rle_packets
    uint8_t nrepeat = 0;

    for(int j = 0; j < header.img.h; j++) {
        for(int i = 0; i < header.img.w; i++) {
            if(header.hasRLE()) {
                if(!nrepeat) {
                    uint8_t rle_packet;
                    file.read(&rle_packet, 1, 1);
                    repeat = rle_packet & RLE_FLAG_MASK;
                    nrepeat = rle_packet & RLE_REPEAT_MASK;

                    file.read(&pixel, 1, header.bytesPerPixel());
                    formatPixel(pixel);
                } else {
                    if(!repeat) { // if we should grab a new pixel (or reuse last sampled)
                        file.read(&pixel, 1, header.bytesPerPixel());
                        formatPixel(pixel);
                    }
                    nrepeat--;
                }
            } else {
                file.read(&pixel, 1, header.bytesPerPixel());
                formatPixel(pixel);
            }

            // if the y-origin is the top (non-zero), flip the image upside down
            image->setPixel(i, header.img.yorg == 0 ? j : header.img.h - j - 1, pixel);
        }
    }
    

    return image;
}
