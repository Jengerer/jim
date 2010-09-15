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
	m_pMap = new stringHashMap();
}

Hashtable::~Hashtable()
{
	stringHashMap::iterator hashIterator;
	for (hashIterator = m_pMap->begin(); hashIterator != m_pMap->end(); hashIterator++)
	{
		Object* thisValue = hashIterator->second;
		if (thisValue != NULL)
		{
			delete thisValue;
			thisValue = NULL;
			hashIterator->second = NULL;
		}
	}

	delete m_pMap;
}

void Hashtable::put(string whichKey, string* whichValue)
{
	StringObject* newObject = new StringObject(whichValue);

	// See if we need to replace it.
	stringHashMap::iterator hashIterator = m_pMap->find(whichKey);
	if (hashIterator != m_pMap->end())
	{
		delete hashIterator->second;
		m_pMap->erase(hashIterator);
	}

	// Insert or re-insert now.
	stringPair thisPair(whichKey, newObject);
	m_pMap->insert(thisPair);
}

void Hashtable::put(string whichKey, Hashtable* whichValue)
{
	stringPair thisPair(whichKey, whichValue);
	m_pMap->insert(thisPair);
}

Object* Hashtable::get(string whichKey)
{
	stringHashMap::iterator hashIterator = m_pMap->find(whichKey);

	if (hashIterator == m_pMap->end())
		throw Exception(whichKey);

	return hashIterator->second;
}

string* Hashtable::getString(string whichKey)
{
	StringObject* thisString = (StringObject*)get(whichKey);
	return thisString->getString();
}

Hashtable* Hashtable::getTable(string whichKey)
{
	return (Hashtable*)get(whichKey);
}

stringHashMap::iterator Hashtable::begin()
{
	return m_pMap->begin();
}

stringHashMap::iterator Hashtable::end()
{
	return m_pMap->end();
}

bool Hashtable::empty() const
{
	return m_pMap->empty();
}

string Hashtable::toString()
{
	return "Hashtable (root: " + (m_pMap->empty() ? "<none>" : m_pMap->begin()->first) + ")";
}