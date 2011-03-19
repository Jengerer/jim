#include "Hashtable.h"

string upper( const string& source )
{
	// Copy the string.
	string result( source );

	string::iterator i;
	for (i = result.begin(); i != result.end(); i++)
		*i = toupper(*i);

	return result;
}

string lower( const string& source )
{
	// Copy the string.
	string result( source );

	string::iterator i;
	for (i = result.begin(); i != result.end(); i++)
		*i = tolower(*i);

	return result;
}

Hashtable::Hashtable()
{
	hashMap_ = new stringMap();
}

Hashtable::~Hashtable()
{
	if (hashMap_ != 0)
	{
		stringMap::iterator hashIterator;
		while (!hashMap_->empty())
		{
			remove( hashMap_->begin() );
		}

		delete hashMap_;
		hashMap_ = 0;
	}
}

bool Hashtable::contains( const string& key ) const
{
	return hashMap_->find( key ) != hashMap_->end();
}

void Hashtable::put( const string& key, boost::any value )
{
	// See if we need to replace it.
	stringMap::iterator i = hashMap_->find( key );
	if (i != hashMap_->end())
	{
		remove( i );
	}

	// Insert or re-insert now.
	stringPair pair( key, value );
	hashMap_->insert( pair );
}

void Hashtable::remove( stringMap::iterator& iter )
{
	try
	{
		// Check if it's a string.
		string* stringValue = boost::any_cast<string*>( iter->second );
		delete stringValue;
		hashMap_->erase( iter );
		return;
	} catch (const boost::bad_any_cast &)
	{
		// This is not a string, do nothing.
	}

	try
	{
		// Check if it's a texture.
		Texture* textureValue = boost::any_cast<Texture*>( iter->second );
		hashMap_->erase( iter );
		return;
	} catch (const boost::bad_any_cast &)
	{
		// This is not a texture, do nothing.
	}

	try
	{
		// Then it must be a table.
		Hashtable* tableValue = boost::any_cast<Hashtable*>( iter->second );

		// Delete it.
		delete tableValue;
		tableValue = NULL;

		// Remove this.
		hashMap_->erase( iter );
	} catch (const boost::bad_any_cast &)
	{
		// It's neither, bad!
		throw Exception( "Failed to deallocate variable from table. Please report this error as soon as possible." );
	}
}

boost::any& Hashtable::get( const string& key ) const
{
	stringMap::iterator iter = hashMap_->find( key );

	if (iter == hashMap_->end())
		throw Exception( key );

	return iter->second;
}

string* Hashtable::getString( const string& key ) const
{
	try
	{
		return boost::any_cast<string*>( get( key ) );
	} catch (const boost::bad_any_cast &)
	{
		throw Exception( "Unexpected type received from key '" + key + "', expected string." );
	}
}

Hashtable* Hashtable::getTable( const string& key ) const
{
	try
	{
		Hashtable* tableValue = boost::any_cast<Hashtable*>( get( key ) );
		return tableValue;
	} catch (const boost::bad_any_cast &)
	{
		throw Exception( "Unexpected type received from key '" + key + "', expected table." );
	}
}

stringMap::iterator Hashtable::find( const string& key )
{
	return hashMap_->find( key );
}

stringMap::iterator Hashtable::begin()
{
	return hashMap_->begin();
}

stringMap::iterator Hashtable::end()
{
	return hashMap_->end();
}

size_t Hashtable::size() const
{
	return hashMap_->size();
}

bool Hashtable::empty() const
{
	return hashMap_->empty();
}