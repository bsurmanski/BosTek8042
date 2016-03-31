#include "stringInput.hpp"

StringInput::StringInput(const String &_str) {
	offset = 0;
	str = _str.dup();
}

long StringInput::getSize() {
	return str.length();
}

int StringInput::seek(long sval) {
	if(offset + sval >= str.length()) return -1;
	offset += sval;
	return 0;
}

int StringInput::set(long sval) {
	if(sval >= str.length()) return -1;
	offset = sval;
	return 0;
}

int StringInput::rset(long sval) {
	long n = getSize() + sval;
	if(n >= str.length()) return -1;
	offset = n;
	return 0;
}

long StringInput::tell() {
	return offset;
}

int StringInput::get() {
	if(offset >= str.length()) return -1;
	return str[offset++];
}

int StringInput::peek(int ahead) {
	if((offset + ahead) >= str.length()) return -1;
	return str[offset + ahead];
}

long StringInput::read(void *buf, long sz, long nmemb) {
	return str.copy((char*) buf, sz*nmemb, offset);
}

bool StringInput::eof() {
	return offset >= str.length();
}

String StringInput::getName() const {
    return "string-constant";
}
