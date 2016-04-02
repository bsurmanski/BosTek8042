#include "char.hpp"

#include <assert.h>

char Char::lowercase(char c) {
    if(c >= 'A' && c <= 'Z') {
        return c - 'A' + 'a';
    }
    return c;
}

bool Char::isNonDigit(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Char::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Char::isIdChar(char c) {
    return isNonDigit(c) || isDigit(c);
}

bool Char::isSign(char c) {
    return c == '-' || c == '+';
}

bool Char::isAsciiPrintable(char c) {
    return c >= 0x20 && c < 0x7f;
}

bool Char::isBinDigit(char c) {
    return c == '0' || c == '1';
}

bool Char::isOctDigit(char c) {
    return c >= '0' && c <= '7';
}

bool Char::isHexDigit(char c) {
    char low = lowercase(c);
    return (c >= '0' && c <= '9') || (low >= 'a' && low <= 'f');
}

bool Char::isDecDigit(char c) {
	return c >= '0' && c <= '9';
}

int Char::hextoi(char c) {
	assert(isHexDigit(c) && "invalid hex digit");
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'a' && c <= 'f') return c - 'a' + 10;
	if(c >= 'A' && c <= 'F') return c - 'A' + 10;
	assert(false);
}

int Char::octtoi(char c) {
	assert(isOctDigit(c) && "invalid oct digit");
	return c - '0';
}

int Char::dectoi(char c) {
	assert(isDecDigit(c) && "invalid dec digit");
	return c - '0';
}

int Char::bintoi(char c) {
	assert(isBinDigit(c) && "invalid binary digit");
	return c - '0';
}

char Char::dectochar(int d) {
    assert(d < 10 && d >= 0);
    return d + '0';
}

char Char::hextochar(int d) {
    assert(d < 16 && d >= 0);
    if(d >= 10) {
        return d - 10 + 'A';
    }
    return d + '0';
}

bool Char::isWhitespace(char c) {
	return c == ' ' || c == '\t' || c == '\v' || c == '\f';
}

bool Char::isEndOfLine(char c) {
	return c == '\n' || c == '\r';
}

bool Char::isExponentChar(char c) {
	return c == 'e' || c == 'E' || c == 'p' || c == 'P';
}

bool Char::isBinExponentChar(char c) {
	return c == 'p' || c == 'P';
}

bool Char::isPunctuatorChar(char c) {
	switch(c) {
		case '~':
		case '!':
		case '%':
		case '^':
		case '&':
		case '*':
		case '(':
		case ')':
		case '-':
		case '+':
		case '=':
		case '[':
		case ']':
		case '{':
		case '}':
		case '|':
		case '/':
		case '<':
		case '>':
		case ';':
		case ':':
		case '.':
		case ',':
			return true;
		default:
			return false;
	}
}
