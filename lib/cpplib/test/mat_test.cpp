#include <gtest/gtest.h>
#include <math.h>
#include "geom/mat.hpp"

TEST(Mat, Create) {
    Mat4 m;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(i == j) {
                EXPECT_FLOAT_EQ(m.get(i, j), 1.0f);
            } else {
                EXPECT_FLOAT_EQ(m.get(i, j), 0.0f);
            }
        }
    }
}

TEST(Mat, GetSet) {
    Mat4 m;
    m.set(1, 2, 10.0f);
    EXPECT_FLOAT_EQ(m.get(1, 2), 10.0f);
}

TEST(Mat, Zero) {
    Mat4 m;
    m.zero();
    for(int i = 0; i < 16; i++) {
        EXPECT_FLOAT_EQ(m[i], 0.0f);
    }
}

TEST(Mat, Mul) {
    Mat4 m;
    for(int i = 0; i < 16; i++) {
        m[i] = i;
    }

    Mat4 permute;
    permute.zero();
    permute.set(2, 0, 1.0f);
    permute.set(0, 1, 1.0f);
    permute.set(3, 2, 1.0f);
    permute.set(1, 3, 1.0f);
    m = m * permute;

    EXPECT_FLOAT_EQ(m.get(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(m.get(0, 1), 5.0f);
    EXPECT_FLOAT_EQ(m.get(0, 2), 9.0f);
    EXPECT_FLOAT_EQ(m.get(0, 3), 13.0f);

    EXPECT_FLOAT_EQ(m.get(1, 0), 3.0f);
    EXPECT_FLOAT_EQ(m.get(1, 1), 7.0f);
    EXPECT_FLOAT_EQ(m.get(1, 2), 11.0f);
    EXPECT_FLOAT_EQ(m.get(1, 3), 15.0f);

    EXPECT_FLOAT_EQ(m.get(2, 0), 0.0f);
    EXPECT_FLOAT_EQ(m.get(2, 1), 4.0f);
    EXPECT_FLOAT_EQ(m.get(2, 2), 8.0f);
    EXPECT_FLOAT_EQ(m.get(2, 3), 12.0f);

    EXPECT_FLOAT_EQ(m.get(3, 0), 2.0f);
    EXPECT_FLOAT_EQ(m.get(3, 1), 6.0f);
    EXPECT_FLOAT_EQ(m.get(3, 2), 10.0f);
    EXPECT_FLOAT_EQ(m.get(3, 3), 14.0f);
}

TEST(Mat, Scale) {
    Mat4 m;
    for(int i = 0; i < 16; i++) {
        m[i] = 1.0f;
    }

    m.scale(2, 2, 2);

    for(int i = 0; i < 12; i++) {
        EXPECT_FLOAT_EQ(m[i], 2.0f);
    }
}
