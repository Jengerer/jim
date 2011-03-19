#pragma once

#include "Texture.h"
#include "Exception.h"

#include <hash_map>
#include <string>
#include <boost/any.hpp>

using namespace std;

string lower(const string& source);
string upper(const string& source);

// The following class defines a hash function for strings.
class stringHasher : public stdext::hash_compare<string>
{
public:
	// Inspired by the java.lang.String.hashCode() algorithm.
	size_t operator() (const string& whichString) const
	{
		size_t result = 0;
		string::const_iterator i, end;

		string newString = lower(whichString);
		for(i = newString.begin(), end = newString.end(); i != end; i++)
			result = 31 * result + (*i);

		return result;
	}

	bool operator() (const string& s1, const string& s2) const
	{
		return lower(s1) < lower(s2);
	}
};

// For easier type usage.
typedef stdext::hash_map<string, boost::any, stringHasher>	stringMap;
typedef stdext::pair<string, boost::any>					stringPair;

// TODO: Add integer support.
//typedef stdext::hash_map<int, boost::any>					intAnyMap;
//typedef stdext::pair<int, boost::any>						intAnyPair;

class Hashtable
{
public:
	Hashtable();
	~Hashtable();

	// Adding and removing member.
	void				put( const string& key, boost::any obj );
	void				remove( stringMap::iterator& iter );

	// Member getter.
	boost::any&			get( const string& key ) const;
	bool				contains( const string& key ) const;
	string*				getString( const string& key ) const;
	Hashtable*			getTable( const string& key ) const;
	
	// For iteration.
	stringMap::iterator	find( const string& key );
	stringMap::iterator	begin();
	stringMap::iterator	end();

	// Simple attribute getter.
	size_t				size() const;
	bool				empty() const;
	
private:
	stringMap*			hashMap_;
};