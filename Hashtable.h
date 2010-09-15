#pragma once

#include "Object.h"
#include "StringObject.h"
#include "Exception.h"

#include <hash_map>
#include <string>

using namespace std;

string toLower(const string& whichString);
string toUpper(const string& whichString);

// The following class defines a hash function for strings 
class stringHasher : public stdext::hash_compare<string>
{
public:
	// Inspired by the java.lang.String.hashCode() algorithm.
	size_t operator() (const string& whichString) const
	{
		size_t hNum = 0;
		string::const_iterator pIter, pEnd;

		string newString = toLower(whichString);
		for(pIter = newString.begin(), pEnd = newString.end(); pIter != pEnd; ++pIter)
			hNum = 31 * hNum + (*pIter);

		return hNum;
	}

	bool operator() (const string& stringOne, const string& stringTwo) const
	{
		return toLower(stringOne) < toLower(stringTwo);
	}
};

typedef stdext::hash_map<string, Object*, stringHasher> stringHashMap;
typedef stdext::pair<string, Object*> stringPair;

class Hashtable: public Object
{
public:
	Hashtable();
	virtual ~Hashtable();

	void put(string whichKey, string* whichObject);
	void put(string whichKey, Hashtable* whichObject);

	Object* get(string whichKey);
	string* getString(string whichKey);
	Hashtable* getTable(string whichTable);

	stringHashMap::iterator begin();
	stringHashMap::iterator end();
	bool empty() const;

	virtual string toString();
	
private:
	stringHashMap* m_pMap;
};