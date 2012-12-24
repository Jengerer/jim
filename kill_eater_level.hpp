#ifndef KILL_EATER_LEVEL_HPP
#define KILL_EATER_LEVEL_HPP

#include <string/string.hpp>
#include "steam/SteamTypes.h"

/*
 * Item definition storage class.
 */
class KillEaterLevel
{

public:

	KillEaterLevel(
		uint32 score,
		JUTIL::String* prefix);
	~KillEaterLevel( void );

	// Item information attributes.
	uint32 get_score( void ) const;
	const JUTIL::String* get_prefix( void ) const;

private:

	// Private attribute setters.
	void set_score( uint32 score );
	void set_prefix( JUTIL::String* prefix );

private:
	uint32 score_;
	JUTIL::String* prefix_;
    // Private item information attributes.
};

#endif // KILL_EATER_LEVEL_HPP