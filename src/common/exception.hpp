#ifndef _EXCEPTION_HPP
#define _EXCEPTION_HPP

#include "common/string.hpp"
#include <exception>

class Exception : public std::exception {
    protected:
	String message;
    const char *whatStr;

	public:
	Exception(const String &_message);
	virtual ~Exception() throw();
	String &getMessage();
    virtual const char* what() const throw();
};

#endif
