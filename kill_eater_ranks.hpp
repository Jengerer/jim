#ifndef KILL_EATER_RANKS_HPP
#define KILL_EATER_RANKS_HPP

#include <string/string.hpp>
#include <containers/vector.hpp>
#include "kill_eater_level.hpp"

/*
 * Class for storing a set of ranks.
 * Represents generic strange counter type (e.g. killing) and many kill eater
 * types can be mapped to one of these objects (e.g. killing scouts, killing medics).
 */
class KillEaterRanks
{

public:

	KillEaterRanks( void );
	~KillEaterRanks( void );

	// Level functions for this rank type.
	bool add_level( KillEaterLevel* level );
	size_t get_level_count( void ) const;
	const KillEaterLevel* get_level( size_t index ) const;
	const KillEaterLevel* find_level( uint32 score ) const;

private:

	// Kill eater levels for this rank type.
	JUTIL::Vector<KillEaterLevel*> levels_;

};

#endif // KILL_EATER_RANKS_HPP