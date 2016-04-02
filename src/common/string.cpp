#include "common/char.hpp"
#include "common/string.hpp"
#include "common/exception.hpp"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) > (B) ? (B) : (A))

_StringData *_StringData::retain() {
	refcount++;
	return this;
}

bool _StringData::release() {
	refcount--;
	if(refcount <= 0) {
		free(this);
		return true;
	}
	return false;
}

String::String() {
	len = 0;
	capacity = STRING_LONG;
}

String::String(const char *str) {
	size_t strl = strlen(str);
	len = strl;
	capacity = MAX(len, STRING_LONG);
	if(len > STRING_LONG) {
		refdata = (_StringData*) malloc(capacity + sizeof(_StringData));
		refdata->refcount = 1;
		memcpy(refdata->data, str, len);
	} else {
		memcpy(data, str, len);
	}
}

String::String(const String& o) {
	len = o.len;
	capacity = o.capacity;
	if(o.isLong()) {
		refdata = o.refdata->retain();
	} else {
		memcpy(data, o.data, o.len);
	}
}

String::String(char c) {
	len = 0;
	capacity = STRING_LONG;
	append(c);
}

String::~String() {
	if(isLong()) {
		refdata->release();
	}
}

String String::fromInt(long long i) {
    String ret;

    bool neg = i < 0;

    ret = fromUInt((unsigned long long) abs(i));

    if(neg) ret.prepend('-');
    return ret;
}

String String::fromUInt(unsigned long long i) {
    String ret;

    do {
        ret.prepend(Char::dectochar(i % 10));
        i /= 10;
    } while(i != 0);

    return ret;
}

String String::fromFloat(long double d) {
    String str;
    str.resize(32);
    str.len = snprintf(str.dataPtr(), 32, "%Lf", d);
    return str;
}

bool String::isLong() const {
	return capacity > STRING_LONG;
}

void String::resize(int cap) {
	if(cap > capacity && cap > STRING_LONG) {
		_StringData *ndata = (_StringData*) malloc(cap + sizeof(_StringData));
		assert(ndata);
		ndata->refcount = 1;
		if(isLong()) {
			memcpy(ndata->data, refdata->data, len);
			refdata->release();
		} else {
			memcpy(ndata->data, &this->data[0], len);
		}
		capacity = cap;
		refdata = ndata;
	} else if(capacity > STRING_LONG && cap < STRING_LONG) { // we are shrinking across 'LONG' boundary
		_StringData *old = refdata;
		memcpy(data, refdata->data, len);
		old->release();
	}
}

void String::clear() {
	resize(0);
}

size_t String::length() const {
	return len;
}

String &String::operator=(const String &o) {
	if(isLong()) {
		refdata->release();
	}

	len = o.len;
	capacity = o.capacity;
	if(o.isLong()) {
		refdata = o.refdata->retain();
	} else {
		memcpy(data, o.data, o.len);
	}

	return *this;
}

char &String::operator[](int i) {
	return charAt(i);
}

const char &String::operator[](int i) const {
	return charAt(i);
}

bool String::operator==(const String &o) const {
	return equals(o);
}


bool String::operator!=(const String &o) const {
	return !equals(o);
}

bool String::operator<(const String &o) const {
	return compare(o) < 0;
}

bool String::operator>(const String &o) const {
	return compare(o) > 0;
}

bool String::operator<=(const String &o) const {
	return compare(o) <= 0;
}

bool String::operator>=(const String &o) const {
	return compare(o) >= 0;
}

void String::operator+=(char c) {
	append(c);
}

void String::operator+=(const String &o) {
	append(o);
}

String String::operator+(String o) {
    String str;
    str.append(*this);
    str.append(o);
    return str;
}

bool String::equals(const String &o) const {
	return !compare(o);
}

const char &String::charAt(int i) const {
	if(i >= capacity) {
		throw Exception("index out of bounds");
	}

	if(isLong()) {
		return refdata->data[i];
	} else {
		return data[i];
	}
}

char &String::charAt(int i) {
	if(i >= capacity) {
		throw Exception("index out of bounds");
	}

	if(isLong()) {
		return refdata->data[i];
	} else {
		return data[i];
	}
}

int String::compare(const String &o) const {
	size_t l1 = length();
	size_t l2 = o.length();
	int cmp = memcmp(dataPtr(), o.dataPtr(), MIN(l1, l2));

	if(!cmp && length() < o.length()) {
		return -1;
	} else if(!cmp && length() > o.length()) {
		return 1;
	}

	return cmp;
}

void String::append(char c) {
	if(len == capacity) {
		resize(capacity+STRING_APPEND_PAD);
	}
	charAt(len) = c;
	len++;
}

void String::append(const String &o) {
	if(length() + o.length() > capacity) {
		resize(length() + o.length()+STRING_APPEND_PAD);
	}

	for(int i = 0; i < o.length(); i++) {
		charAt(len + i) = o.charAt(i);
	}

	len += o.length();
}

void String::prepend(char c) {
    if(len == capacity) {
        resize(capacity + STRING_APPEND_PAD);
    }

    for(int i = length(); i > 0; i--) {
		charAt(i) = charAt(i-1);
    }
    charAt(0) = c;
    len++;
}

void String::prepend(const String &o) {
    while(len + o.length() >= capacity) {
        resize(capacity + STRING_APPEND_PAD);
    }

    for(int i = length() + o.length() - 1; i >= o.length(); i--) {
		charAt(i) = charAt(i-o.length());
    }

    for(int i = o.length() - 1; i >= 0; i--) {
        charAt(i) = o.charAt(i);
    }
    len+=o.length();
}

char *String::c_str() {
	if(len + 1 >= capacity) {
		resize(len+STRING_APPEND_PAD);
	}
	charAt(len) = '\0';
	return dataPtr();
}

char *String::dataPtr() {
	if(isLong()) {
		return refdata->data;
	} else {
		return data;
	}
}

const char *String::dataPtr() const {
	if(isLong()) {
		return refdata->data;
	} else {
		return data;
	}
}

size_t String::copy(char *dst, size_t len, size_t pos) const {
    size_t n = len;
    if(length() < pos + len) {
        n = length() - pos;
    }

	if(isLong()) {
		memcpy(dst, &refdata->data[pos], n);
	} else {
		memcpy(dst, &data[pos], n);
	}
    return n;
}

String String::substring(int start, int len) const {
    String ret;

    for(int i = start; (len < 0 || i < (start + len)) && i < length(); i++) {
        ret += charAt(i);
    }

    return ret;
}

int String::indexOf(int c, int startAt) const {
    for(int i = startAt; i < length(); i++) {
        if(charAt(i) == c) return i;
    }

    return -1;
}

int String::lastIndexOf(int c, int startAt) const {
    if(startAt < 0) startAt = length() - 1;

    for(int i = startAt; i >= 0; i--) {
        if(charAt(i) == c) return i;
    }

    return -1;
}

int String::count(char ch) {
    int c = 0;
    for(int i = 0; i < length(); i++) {
        if (charAt(i) == ch) c++;
    }
    return c;
}

bool String::empty() const {
	return len <= 0;
}

String String::dup() const {
	String o;
	o.resize(capacity);
	copy(o.dataPtr(), length(), 0);
	o.len = length();
	return o;
}

String String::escapedString() const {
    String ret;
    bool escaped = false;
    for(int i = 0; i < length(); i++) {
        char c = charAt(i);
        if(Char::isAsciiPrintable(c)) {
            if(escaped && Char::isHexDigit(c)) ret.append("\\ ");

            if(c == '\\') {
                ret.append("\\\\");
            } else {
                ret.append(c);
            }
            escaped = false;
        } else {
            if(!escaped) ret.append("\\");
            ret.append(Char::hextochar((c >> 4) & 0x0f));
            ret.append(Char::hextochar(c & 0x0f));
            escaped = true;
        }
    }
    return ret;
}

std::ostream& operator<<(std::ostream &os, const String &str) {
    String dup = str;
    return os << dup.c_str();
}
