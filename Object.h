#pragma once

#include <sstream>
#include <string>
#include <iostream>

using namespace std;

class Object
{
public:
	virtual ~Object();
	virtual string toString();
};