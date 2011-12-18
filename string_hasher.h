#ifndef STRING_HASHER_H
#define STRING_HASHER_H

#include <hash_map>
#include <string>

class StringHasher : public stdext::hash_compare<std::string>
{

public:

	// Inspired by the java.lang.String.hashCode() algorithm.
	size_t operator()( const std::string& input ) const
	{
		size_t result = 0;
		for (auto i = input.begin(), end = input.end(); i != end; ++i) {
			result = 31 * result + tolower(*i);
		}

		return result;
	}

	bool operator() ( const std::string& s1, const std::string& s2 ) const
	{
		return (*this)(s1) < (*this)(s2);
	}

};

#endif // STRING_HASHER_H