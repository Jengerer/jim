#ifndef KILL_EATER_TYPE_HPP
#define KILL_EATER_TYPE_HPP

#include "kill_eater_ranks.hpp"
#include <string/string.hpp>

/*
 * Strange type class.
 */
class KillEaterType
{

public:

	KillEaterType( JUTIL::String* description, const KillEaterRanks* levels );
	~KillEaterType( void );

	// Getters for this strange type.
	const JUTIL::String* get_description( void ) const;
	const KillEaterRanks* get_levels( void ) const;

private:

	// Private attribute setters.
	void set_description( JUTIL::String* description );
	void set_levels( const KillEaterRanks* levels );

private:

    // Private item information attributes.
	JUTIL::String* description_;
	const KillEaterRanks* levels_;
};

#endif // KILL_EATER_TYPE_HPP