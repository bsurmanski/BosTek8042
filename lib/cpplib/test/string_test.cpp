#include <gtest/gtest.h>

#include "common/string.hpp"

TEST(String, Create) {
	String str("Hello");
	EXPECT_FALSE(memcmp(str.c_str(), "Hello", 5));
}

TEST(String, FromInt) {
    String str1 = String::fromInt(1000);
    EXPECT_EQ(str1, "1000");

    str1 = String::fromInt(1234567890);
    EXPECT_EQ(str1, "1234567890");

    str1 = String::fromInt(-1);
    EXPECT_EQ(str1, "-1");

    str1 = String::fromInt(-987123);
    EXPECT_EQ(str1, "-987123");

    str1 = String::fromInt(0);
    EXPECT_EQ(str1, "0");
}

TEST(String, Compare) {
	String str1("SomeString");
	String str2("SomeString");
	String str3("somestring");

	String lstr1("123456789012345678901234567890123456789");
	String lstr2("123456789012345678901234567890123456789");
	String lstr3("123456789012345678901234567890123456781");
	EXPECT_EQ(str1, str2);
	EXPECT_NE(str1, str3);
	EXPECT_EQ(lstr1, lstr2);
	EXPECT_NE(lstr1, lstr3);

	String str4("aaab");
	String str5("aaac");
	String str6("aaaca");
	EXPECT_LT(str4, str5);
	EXPECT_GT(str5, str4);
	EXPECT_LT(str5, str6);

    String str7("My Long String");
    String str8("My");
    EXPECT_NE(str7, str8);
}

TEST(String, Length) {
	String str1("123");
	String str2("Hello");
	String str3("Here is a long string. it is intended to be longer than 32 characters.");
	String str4;
	EXPECT_EQ(str1.length(), 3);
	EXPECT_EQ(str2.length(), 5);
	EXPECT_EQ(str3.length(), 70);
	EXPECT_EQ(str4.length(), 0);
}

TEST(String, Append) {
	String str1;
	str1.append('a');
	EXPECT_EQ(str1, "a");
	str1.append("lways");
	EXPECT_EQ(str1, "always");
	str1.append(" bet on black. always and forever");
	EXPECT_EQ(str1, "always bet on black. always and forever");
}

TEST(String, Prepend) {
    String str1 = "Hello";
    str1.prepend('a');
    EXPECT_EQ(str1, "aHello");

    String str2 = " World";
    str2.prepend(str1);
    EXPECT_EQ(str2, "aHello World");

    String str3 = "A Quite Long String To Test With. Quite Long Indeed";
    String str4 = str3.dup();
    str4.prepend(str2);
    str2.append(str3);

    EXPECT_STREQ(str2.c_str(), str4.c_str());
}

TEST(String, Cstr) {
	String str1;
	str1.append("1234567890");
	EXPECT_STREQ(str1.c_str(), "1234567890");
	str1.append("123456789012345678901234567890");
	EXPECT_STREQ(str1.c_str(), "1234567890123456789012345678901234567890");
}

TEST(String, Empty) {
	String str1;
	EXPECT_TRUE(str1.empty());
	str1.append('\0');
	EXPECT_FALSE(str1.empty());
}

TEST(String, Copy) {
	char cstr1[32];
	String str1("Hello");
	str1.copy(cstr1, 5);
	cstr1[5] = '\0';
	EXPECT_STREQ("Hello", cstr1);
	str1.copy(cstr1, 2, 2);
	EXPECT_STREQ("llllo", cstr1);
}

TEST(String, Dup) {
	String st1("test");
	String st2 = st1;
	String st3 = st1.dup();

	EXPECT_EQ(st1, st2);
	EXPECT_EQ(st1, st3);
	st1[0] = 'b';
	EXPECT_NE(st1, st3);

	String st4("1234567890123456789012345678901234567890");
	String st5 = st4;
	String st6 = st4.dup();
	st4[0] = '0';
	EXPECT_EQ(st4, st5);
	EXPECT_NE(st4, st6);
}

TEST(String, Substring) {
    String st("My Long String");
    String sub = st.substring(0, 2);
    EXPECT_EQ(String("My"), sub);
    sub = st.substring(3, 4);
    EXPECT_EQ(String("Long"), sub);
}

TEST(String, IndexOf) {
    String st("3.141592654");
    EXPECT_EQ(3, st.indexOf('4'));
    EXPECT_EQ(10, st.indexOf('4', 4));
    EXPECT_EQ(-1, st.indexOf('7'));
}

TEST(String, Count) {
    EXPECT_EQ(2, String("hello").count('l'));
    EXPECT_EQ(1, String("hello").count('o'));
    EXPECT_EQ(2, String("a longer string").count('g'));
    EXPECT_EQ(3, String("a \t string \n with \n escape \n characters").count('\n'));
    EXPECT_EQ(0, String("").count('a'));
}

TEST(String, Escape) {
    EXPECT_EQ(String("somestring"), String("somestring").escapedString());
    EXPECT_EQ(String("some\\0Astring"), String("some\nstring").escapedString());
}
