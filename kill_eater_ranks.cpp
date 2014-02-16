#include "kill_eater_ranks.hpp"
#include "steam/SteamTypes.h"

/*
 * Constructor for storing item information by item parameters.
 */
KillEaterRanks::KillEaterRanks( void )
{
}

/*
 * Item information destructor.
 */
KillEaterRanks::~KillEaterRanks( void )
{
	size_t i;
    size_t length = levels_.get_length();
    for (i = 0; i < length; ++i) {
        KillEaterLevel* level = levels_.at( i );
        JUTIL::BaseAllocator::destroy( level );
    }
    levels_.clear();
}

/*
 * Add a level to this rank type.
 */
bool KillEaterRanks::add_level( KillEaterLevel* level )
{
	return levels_.push( level );
}

/*
 * Get number of levels in this rank type.
 */
size_t KillEaterRanks::get_level_count( void ) const
{
	return levels_.get_length();
}

/*
 * Get level by index.
 */
const KillEaterLevel* KillEaterRanks::get_level( size_t index ) const
{
	return levels_.at( index );
}

/*
 * Find a level by a given score.
 */
const KillEaterLevel* KillEaterRanks::find_level( uint32 score ) const
{
    size_t i;
    size_t length = get_level_count();
	const KillEaterLevel* level;
    for (i = 0; i < length; ++i) {
        level = get_level( i );
		if (score < level->get_score()){
			return level;
		}
	}

	// Not found, return the highest level
	return level;
}
