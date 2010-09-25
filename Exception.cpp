#include "Exception.h"

Exception::Exception( const std::string& message )
{
	message_ = message;
}

std::string Exception::getMessage()
{
	return message_;
}