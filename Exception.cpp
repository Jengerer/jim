#include "Exception.h"

Exception::Exception(const string exceptionMsg)
{
	setMessage(exceptionMsg);
}

string Exception::getMessage()
{
	return m_strMessage;
}

void Exception::setMessage(const string exceptionMsg)
{
	m_strMessage = exceptionMsg;
}