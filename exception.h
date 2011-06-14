#pragma once

#include <string>
using namespace std;

class Exception
{
public:
	Exception( const string& message );
	const string* getMessage();

private:
	string message_;
};