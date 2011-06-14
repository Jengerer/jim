#include "exception.h"

Exception::Exception( const string& message )
{
	message_ = message;
}

const string* Exception::getMessage()
{
	return &message_;
}