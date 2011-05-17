#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <boost/regex.hpp>
#include "Hashtable.h"
#include "Exception.h"

using namespace std;

size_t getKey(string whichLine, string* destKey);
bool getKeyValue(string whichLine, string* destKey, string* destValue);

class KeyValueParser
{

public:

	static Hashtable* Parse( const string& inputString );

};