#pragma once

#include "Texture.h"
#include "Exception.h"

#include <hash_map>
#include <string>
#include <boost/any.hpp>

// The following class defines a hash function for strings.
class StringHasher : public std::hash_compare<string>
{
public:
	// Adapted from the java.lang.String.hashCode() algorithm.
	size_t operator() ( const string& whichString ) const
	{
		size_t result = 0;
		string::const_iterator i, end;
		for (i = whichString.begin(), end = whichString.end(); i != end; i++) {
			result = 31 * result + tolower( *i );
		}

		return result;
	}

	bool operator() ( const string& s1, const string& s2 ) const
	{
		return operator()(s1) < operator()(s2);
	}
};

// For easier type usage.
// TODO: Maybe add int key support.
typedef std::hash_map<string, boost::any, StringHasher>	stringMap;
typedef std::pair<string, boost::any>					stringPair;

class Hashtable
{
public:
	Hashtable();
	virtual ~Hashtable();

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