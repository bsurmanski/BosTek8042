#include <gtest/gtest.h>

#include "cpplib/geom/ball.hpp"

TEST(Ball, Collide) {
    Ball b1(1, 0, 0, 1);
    Ball b2(2, 0, 0, 1);
    Ball b3(3.1, 0, 0, 1);
    Ball b4(1, 2, 3, 3.5);
    Ball b5(3, 4, 5, 0.1);
    EXPECT_TRUE(b1.collides(b2));
    EXPECT_TRUE(b2.collides(b1));
    EXPECT_FALSE(b1.collides(b3));
    EXPECT_FALSE(b3.collides(b1));
    EXPECT_TRUE(b4.collides(b5));
}
