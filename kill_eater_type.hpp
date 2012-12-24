#ifndef KILL_EATER_TYPE_HPP
#define KILL_EATER_TYPE_HPP

#include "kill_eater_rank.hpp"
#include <string/string.hpp>

/*
 * Strange type class.
 */
class KillEaterType
{

public:

	KillEaterType(
		JUTIL::String* description,
		const KillEaterRank* levels );
	~KillEaterType( void );

	const JUTIL::String* get_description( void ) const;
	const KillEaterRank* get_levels( void ) const;

private:

	// Private attribute setters.
	void set_description( JUTIL::String* description );
	void set_levels( const KillEaterRank* levels );

private:

    // Private item information attributes.
	JUTIL::String* description_;
	const KillEaterRank* levels_;
};

#endif // KILL_EATER_TYPE_HPP