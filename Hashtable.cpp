#include "Hashtable.h"

string toUpper(const string& whichString)
{
	// Copy the string.
	string newString(whichString);

	string::iterator strIterator;
	for (strIterator = newString.begin(); strIterator != newString.end(); strIterator++)
		*strIterator = toupper(*strIterator);

	return newString;
}

string toLower(const string& whichString)
{
	// Copy the string.
	string newString(whichString);

	string::iterator strIterator;
	for (strIterator = newString.begin(); strIterator != newString.end(); strIterator++)
		*strIterator = tolower(*strIterator);

	return newString;
}

Hashtable::Hashtable()
{
	m_pMap = new stringAnyMap();
}

Hashtable::~Hashtable()
{
	stringAnyMap::iterator hashIterator;
	while (!m_pMap->empty())
	{
		remove(m_pMap->begin());
	}

	delete m_pMap;
}

void Hashtable::put(const string& whichKey, boost::any whichValue)
{
	// See if we need to replace it.
	stringAnyMap::iterator hashIterator = m_pMap->find(whichKey);
	if (hashIterator != m_pMap->end())
	{
		remove(hashIterator);
	}

	// Insert or re-insert now.
	stringAnyPair thisPair(whichKey, whichValue);
	m_pMap->insert(thisPair);
}

void Hashtable::remove(stringAnyMap::iterator& pIterator)
{
	try
	{
		// Check if it's a string.
		string* thisString = boost::any_cast<string*>(pIterator->second);
		delete thisString;
		thisString = NULL;

		// It worked, remove this and exit.
		m_pMap->erase(pIterator);
		return;
	} catch (const boost::bad_any_cast &)
	{
		// This is not a string, do nothing.
	}

	try
	{
		// Check if it's a texture.
		Texture* thisTexture = boost::any_cast<Texture*>(pIterator->second);
		
		// Textures will be automatically released at the end, just remove.
		m_pMap->erase(pIterator);
		return;
	} catch (const boost::bad_any_cast &)
	{
		// This is not a texture, do nothing.
	}

	try
	{
		// Then it must be a table.
		Hashtable* thisTable = boost::any_cast<Hashtable*>(pIterator->second);

		delete thisTable;
		thisTable = NULL;

		// Remove this.
		m_pMap->erase(pIterator);
	} catch (const boost::bad_any_cast &)
	{
		// It's neither, bad!
		throw Exception("Failed to deallocate variable from table. Please report this error as soon as possible.");
	}
}

boost::any& Hashtable::get(const string& whichKey)
{
	stringAnyMap::iterator hashIterator = m_pMap->find(whichKey);

	if (hashIterator == m_pMap->end())
		throw Exception(whichKey);

	return hashIterator->second;
}

string* Hashtable::getString(const string& whichKey)
{
	try
	{
		return boost::any_cast<string*>(get(whichKey));
	} catch (const boost::bad_any_cast &)
	{
		throw Exception("Unexpected type received from key '" + whichKey + "', expected string.");
	}
}

Hashtable* Hashtable::getTable(const string& whichKey)
{
	try
	{
		Hashtable* thisMap = boost::any_cast<Hashtable*>(get(whichKey));
		return thisMap;
	} catch (const boost::bad_any_cast &)
	{
		throw Exception("Unexpected type received from key '" + whichKey + "', expected table.");
	}
}

stringAnyMap::iterator Hashtable::find(const string& whichKey)
{
	return m_pMap->find(whichKey);
}

stringAnyMap::iterator Hashtable::begin()
{
	return m_pMap->begin();
}

stringAnyMap::iterator Hashtable::end()
{
	return m_pMap->end();
}

bool Hashtable::empty() const
{
	return m_pMap->empty();
}