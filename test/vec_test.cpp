#include <gtest/gtest.h>

#include <math.h>

#include "geom/vec.hpp"
#include "geom/mat.hpp"

TEST(Vec, Create) {
    Vec4 a(1, 2, 3);
    EXPECT_FLOAT_EQ(a.x, 1);
    EXPECT_FLOAT_EQ(a.y, 2);
    EXPECT_FLOAT_EQ(a.z, 3);
}

TEST(Vec, VMul) {
    Vec4 a(1, 2, 3);
    Mat4 t;
    t.zero();
    t.set(0, 1, 1.0f);
    t.set(1, 0, 1.0f);
    t.set(2, 3, 1.0f);
    t.set(3, 2, 1.0f);
    Vec4 b = a.mul(t);
    EXPECT_FLOAT_EQ(b[0], 2.0f);
    EXPECT_FLOAT_EQ(b[1], 1.0f);
    EXPECT_FLOAT_EQ(b[2], 0.0f);
    EXPECT_FLOAT_EQ(b[3], 3.0f);

    b = Vec4(1, 2, 3) * t;
    EXPECT_FLOAT_EQ(b[0], 2.0f);
    EXPECT_FLOAT_EQ(b[1], 1.0f);
    EXPECT_FLOAT_EQ(b[2], 0.0f);
    EXPECT_FLOAT_EQ(b[3], 3.0f);
}

TEST(Vec, Add) {
    Vec4 a(1, 2, 3);
    Vec4 b(4, 5, 6);
    Vec4 c = a + b;
    EXPECT_FLOAT_EQ(c.x, 5);
    EXPECT_FLOAT_EQ(c.y, 7);
    EXPECT_FLOAT_EQ(c.z, 9);

    a+=b;
    EXPECT_FLOAT_EQ(a.x, 5);
    EXPECT_FLOAT_EQ(a.y, 7);
    EXPECT_FLOAT_EQ(a.z, 9);
}

TEST(Vec, Sub) {
    Vec4 a(1, 2, 3);
    Vec4 b(4, 6, 8);
    Vec4 c = a - b;
    EXPECT_FLOAT_EQ(c.x, -3);
    EXPECT_FLOAT_EQ(c.y, -4);
    EXPECT_FLOAT_EQ(c.z, -5);

    a-=b;
    EXPECT_FLOAT_EQ(a.x, -3);
    EXPECT_FLOAT_EQ(a.y, -4);
    EXPECT_FLOAT_EQ(a.z, -5);
}

TEST(Vec, Scale) {
    Vec4 a(1, 2, 3);
    Vec4 b = a.scaled(5);
    EXPECT_FLOAT_EQ(b.x, 5);
    EXPECT_FLOAT_EQ(b.y, 10);
    EXPECT_FLOAT_EQ(b.z, 15);
}

TEST(Vec, Length) {
    Vec4 a(11, 12, 13);
    EXPECT_FLOAT_EQ(a.lensq(), 434.0);
    EXPECT_FLOAT_EQ(a.length(), sqrt(434.0));
}

TEST(Vec, Distance) {
    Vec4 a(0,0,0);
    Vec4 b(1,2,3);
    Vec4 c(5,6,11);

    EXPECT_FLOAT_EQ(a.distance(a), 0);
    EXPECT_FLOAT_EQ(a.distance(b), b.length());
    EXPECT_FLOAT_EQ(a.distsq(b), b.lensq());
    EXPECT_FLOAT_EQ(b.distsq(c), 96);
    EXPECT_FLOAT_EQ(b.distance(c), sqrt(96.0));
}

TEST(Vec, Normalize) {
    Vec4 a(0.5, 0, 0);
    Vec4 b = a.normalized();
    EXPECT_FLOAT_EQ(b.x, 1);
    EXPECT_FLOAT_EQ(b.y, 0);
    EXPECT_FLOAT_EQ(b.z, 0);

    Vec4 c(1, 2, 3);
    Vec4 d = c.normalized();
    EXPECT_FLOAT_EQ(d.x, 1.0 / sqrt(14.0));
    EXPECT_FLOAT_EQ(d.y, 2.0 / sqrt(14.0));
    EXPECT_FLOAT_EQ(d.z, 3.0 / sqrt(14.0));
    EXPECT_FLOAT_EQ(d.lensq(), 1.0);
}

TEST(Vec, Proj) {
    Vec4 a(2, 0, 0);
    Vec4 b(1, 2, 3);
    Vec4 c = a.proj(a);
    Vec4 d = b.proj(a);

    EXPECT_FLOAT_EQ(c.length(), 2);
    EXPECT_FLOAT_EQ(c.x, 2);
    EXPECT_FLOAT_EQ(d.length(), 1);
    EXPECT_FLOAT_EQ(d.x, 1);
}

TEST(Vec, Orth) {
    Vec4 a(2, 0, 0);
    Vec4 b(1, 2, 3);
    Vec4 c = a.orth(a);
    Vec4 d = b.orth(a);

    EXPECT_FLOAT_EQ(c.length(), 0);
    EXPECT_FLOAT_EQ(d.lensq(), 13);
    EXPECT_FLOAT_EQ(d.x, 0);
    EXPECT_FLOAT_EQ(d.y, 2);
    EXPECT_FLOAT_EQ(d.z, 3);
}

TEST(Vec, Cross) {
    Vec4 a(1, 0, 0);
    Vec4 b(0, 1, 0);
    Vec4 c = a.cross(a);
    Vec4 d = a.cross(b);

    EXPECT_FLOAT_EQ(c.lensq(), 0);
    EXPECT_FLOAT_EQ(d.lensq(), 1);
    EXPECT_FLOAT_EQ(d.z, 1);
}

TEST(Vec, Dot) {
    Vec4 a(1, 2, 3);
    Vec4 b(6, 7, 8);
    EXPECT_FLOAT_EQ(a.dot(b), (6.0 + 14.0 + 24.0));
}
