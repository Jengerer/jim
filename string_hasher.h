#ifndef STRING_HASHER_H
#define STRING_HASHER_H

#include <hash_map>

class StringHasher : public stdext::hash_compare<string>
{

public:

	// Inspired by the java.lang.String.hashCode() algorithm.
	size_t operator()( const string& input ) const
	{
		size_t result = 0;
		string::const_iterator i, end;
		for (auto i = input.begin(), end = input.end(); i != end; ++i) {
			result = 31 * result + tolower(*i);
		}

		return result;
	}

	bool operator() ( const string& s1, const string& s2 ) const
	{
		return operator()( s1 ) < operator()( s2 );
	}

};

#endif // STRING_HASHER_H