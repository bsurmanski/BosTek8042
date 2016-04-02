#include <gtest/gtest.h>

#include "draw/tga.hpp"
#include "common/file.hpp"

TEST(TGA, SimpleRead) {
    File file("tga_simple_test.tga");
    Image *img = loadTga(file);
    ASSERT_NE(img, (Image*) NULL);

    EXPECT_EQ(Pixel(0xff, 0x00, 0xff, 0xff), img->getPixel(0,0));
    EXPECT_EQ(Pixel(0x00, 0xff, 0xff, 0xff), img->getPixel(1,0));
    EXPECT_EQ(Pixel(0xff, 0xff, 0x00, 0xff), img->getPixel(2,0));

    EXPECT_EQ(Pixel(0xff, 0x00, 0x00, 0xff), img->getPixel(0,1));
    EXPECT_EQ(Pixel(0x00, 0xff, 0x00, 0xff), img->getPixel(1,1));
    EXPECT_EQ(Pixel(0x00, 0x00, 0xff, 0xff), img->getPixel(2,1));

    EXPECT_EQ(Pixel(0xff, 0xff, 0xff, 0xff), img->getPixel(0,2));
    EXPECT_EQ(Pixel(0xaa, 0xaa, 0xaa, 0xff), img->getPixel(1,2));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(2,2));
}

TEST(TGA, RLE) {
    File file("tga_rle_test.tga");
    Image *img = loadTga(file);
    ASSERT_NE(img, (Image*) NULL);

    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(0,0));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(1,0));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(2,0));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(3,0));

    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(0,1));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(1,1));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(2,1));
    EXPECT_EQ(Pixel(0xff, 0xff, 0xff, 0xff), img->getPixel(3,1));

    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(0,2));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(1,2));
    EXPECT_EQ(Pixel(0xff, 0xff, 0xff, 0xff), img->getPixel(2,2));
    EXPECT_EQ(Pixel(0xff, 0xff, 0xff, 0xff), img->getPixel(3,2));

    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(0,3));
    EXPECT_EQ(Pixel(0xff, 0xff, 0xff, 0xff), img->getPixel(1,3));
    EXPECT_EQ(Pixel(0xff, 0xff, 0xff, 0xff), img->getPixel(2,3));
    EXPECT_EQ(Pixel(0xff, 0xff, 0xff, 0xff), img->getPixel(3,3));

}

TEST(TGA, Alpha) {
    File file("tga_alpha_test.tga");
    Image *img = loadTga(file);
    ASSERT_NE(img, (Image*) NULL);

    EXPECT_EQ(0x00, img->getPixel(0,0).a);
    EXPECT_EQ(0x00, img->getPixel(1,0).a);
    EXPECT_EQ(0x00, img->getPixel(2,0).a);
    EXPECT_EQ(0x00, img->getPixel(3,0).a);

    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0x7f), img->getPixel(0,1));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0x7f), img->getPixel(1,1));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0x7f), img->getPixel(2,1));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0x7f), img->getPixel(3,1));

    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xbf), img->getPixel(0,2));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xbf), img->getPixel(1,2));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xbf), img->getPixel(2,2));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xbf), img->getPixel(3,2));

    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(0,3));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(1,3));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(2,3));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(3,3));
}

TEST(TGA, OriginTop) {
    File file("tga_origintop_test.tga");
    Image *img = loadTga(file);
    ASSERT_NE(img, (Image*) NULL);

    EXPECT_EQ(Pixel(0xff, 0x00, 0xff, 0xff), img->getPixel(0,0));
    EXPECT_EQ(Pixel(0x00, 0xff, 0xff, 0xff), img->getPixel(1,0));
    EXPECT_EQ(Pixel(0xff, 0xff, 0x00, 0xff), img->getPixel(2,0));

    EXPECT_EQ(Pixel(0xff, 0x00, 0x00, 0xff), img->getPixel(0,1));
    EXPECT_EQ(Pixel(0x00, 0xff, 0x00, 0xff), img->getPixel(1,1));
    EXPECT_EQ(Pixel(0x00, 0x00, 0xff, 0xff), img->getPixel(2,1));

    EXPECT_EQ(Pixel(0xff, 0xff, 0xff, 0xff), img->getPixel(0,2));
    EXPECT_EQ(Pixel(0xaa, 0xaa, 0xaa, 0xff), img->getPixel(1,2));
    EXPECT_EQ(Pixel(0x00, 0x00, 0x00, 0xff), img->getPixel(2,2));
}
