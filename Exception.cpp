#include "Exception.h"

Exception::Exception( const string& message )
{
	message_ = message;
}

const string* Exception::getMessage()
{
	return &message_;
}