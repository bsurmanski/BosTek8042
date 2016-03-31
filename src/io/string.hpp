#ifndef _STRING_HPP
#define _STRING_HPP

#include <stdlib.h>
#include <ostream>

#define STRING_LONG 28
#define STRING_APPEND_PAD 16

struct _StringData {
	int refcount;
	char data[];

	_StringData *retain();
	bool release();
};

class String {
	int capacity;
	int len;

    // if string data is less than STRING_LONG, than the entire string contents
	// are stored in 'data'. This makes creation and deletion of simple strings
	// faster by avoiding memory allocation.
    union {
        char data[STRING_LONG];
		_StringData *refdata;
    };

	bool isLong() const;
	void resize(int cap);

	public:
	String();
	String(const char *str);
	String(const String& o);
	String(char c);
	~String();

    //operator const char*();

    static String fromInt(long long i);
    static String fromUInt(unsigned long long i);
    static String fromFloat(long double d);

	void clear();
	size_t length() const;
	String &operator=(const String &o);
	char &operator[](int i);
	const char &operator[](int i) const;
	bool operator==(const String &o) const;
	bool operator!=(const String &o) const;
	bool operator<(const String &o) const;
	bool operator>(const String &o) const;
	bool operator<=(const String &o) const;
	bool operator>=(const String &o) const;
	void operator+=(char c);
	void operator+=(const String &o);
    String operator+(String o);
	const char &charAt(int i) const;
	char &charAt(int i);
	bool equals(const String &o) const;
	int compare(const String &o) const;
	void append(char c);
	void append(const String &o);
    void prepend(char c);
    void prepend(const String &o);
	char *c_str();
	char *dataPtr();
	const char *dataPtr() const;
	size_t copy(char *dst, size_t len, size_t pos = 0) const;
    String substring(int start, int len = -1) const;
    int indexOf(int c, int startAt = 0) const;
    int lastIndexOf(int c, int startAt = -1) const;
    int count(char ch);
	bool empty() const;
	String dup() const;

    String escapedString() const;
};

std::ostream& operator<<(std::ostream &os, const String &str);

#endif
