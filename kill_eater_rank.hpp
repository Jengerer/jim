#ifndef KILL_EATER_RANK_HPP
#define KILL_EATER_RANK_HPP

#include <string/string.hpp>
#include <containers/vector.hpp>
#include "kill_eater_level.hpp"

/*
 * Strange rank storage class.
 */
class KillEaterRank
{

public:

	KillEaterRank( void );
	~KillEaterRank( void );

	// Item information attributes.
	bool add_level( KillEaterLevel* level );
	size_t get_level_count( void ) const;
	const KillEaterLevel* get_level( size_t index ) const;
	const KillEaterLevel* find_level( uint32 score ) const;

private:

	// Private attribute setters.

private:
	JUTIL::Vector<KillEaterLevel*> levels_;
    // Private item information attributes.
};

#endif // KILL_EATER_RANK_HPP