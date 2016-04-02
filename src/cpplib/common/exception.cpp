#include "exception.hpp"


Exception::Exception(const String &_message) {
	message = _message;
    whatStr = message.c_str();
}

Exception::~Exception() throw() {
}

String &Exception::getMessage() {
	return message;
}

const char *Exception::what() const throw() {
    return whatStr;
}
