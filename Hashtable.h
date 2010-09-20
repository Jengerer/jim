#pragma once

#include "Exception.h"

#include <hash_map>
#include <string>
#include <boost/any.hpp>

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

// For easier type usage.
typedef stdext::hash_map<string, boost::any, stringHasher>	stringAnyMap;
typedef stdext::pair<string, boost::any>					hashPair;

class Hashtable
{
public:
	Hashtable();
	~Hashtable();

	// Adding and removing member.
	void						put(const string& whichKey, boost::any whichObject);
	void						remove(stringAnyMap::iterator& pIterator);

	// Member getter.
	boost::any&					get(const string& whichKey);
	string*						getString(const string& whichKey);
	Hashtable*					getTable(const string& whichTable);
	
	// For iteration.
	stringAnyMap::iterator		begin();
	stringAnyMap::iterator		end();

	// Simple attribute getter.
	bool						empty() const;
	
private:
	stringAnyMap* m_pMap;
};