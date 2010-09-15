#pragma once

#include "Object.h"
#include <string>

using namespace std;

class StringObject: public Object
{
public:
	StringObject(string* newString);
	virtual ~StringObject();

	string* getString();
	virtual string toString();

private:
	string* m_whichString;
};