#pragma once	

#include "StringObject.h"

StringObject::StringObject(string* newString)
{
	m_whichString = newString;
}

StringObject::~StringObject()
{
	if (m_whichString != NULL)
	{
		delete m_whichString;
		m_whichString = NULL;
	}
}

string* StringObject::getString()
{
	return m_whichString;
}

string StringObject::toString()
{
	return "String (" + *m_whichString + ")";
}