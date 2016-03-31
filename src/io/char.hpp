#ifndef _CHAR_HPP
#define _CHAR_HPP

namespace Char {
    char lowercase(char c);
    bool isNonDigit(char c);
    bool isDigit(char c);
    bool isIdChar(char c);
    bool isSign(char c);
    bool isAsciiPrintable(char c);
    bool isBinDigit(char c);
    bool isOctDigit(char c);
    bool isHexDigit(char c);
    bool isDecDigit(char c);
    int hextoi(char c);
    int octtoi(char c);
    int dectoi(char c);
    int bintoi(char c);
    char dectochar(int d);
    char hextochar(int d);
    bool isWhitespace(char c);
    bool isEndOfLine(char c);
    bool isExponentChar(char c);
    bool isBinExponentChar(char c);
    bool isPunctuatorChar(char c);
}

#endif
