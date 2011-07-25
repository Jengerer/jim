#include "exception.h"

Exception::Exception( const string& message )
{
	message_ = message;
}

const string* Exception::getMessage() const
{
	return &message_;
}