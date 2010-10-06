#pragma once

#include <string>

class Exception
{
public:
	Exception( const std::string& message );

	std::string getMessage();

private:
	std::string	message_;
};