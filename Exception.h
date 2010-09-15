#pragma once

#include <string>

using namespace std;

class Exception
{
public:
	Exception(const string exceptionMsg);

	string getMessage();
private:
	// Only the exception gets to set the message.
	void	setMessage(const string exceptionMsg);

	string	m_strMessage;
};