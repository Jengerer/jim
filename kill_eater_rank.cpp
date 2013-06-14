#include "kill_eater_rank.hpp"
#include "steam/SteamTypes.h"

/*
 * Constructor for storing item information by item parameters.
 */
KillEaterRank::KillEaterRank( void )
{
}

/*
 * Item information destructor.
 */
KillEaterRank::~KillEaterRank( void )
{
	size_t i;
    size_t length = levels_.get_length();
    for (i = 0; i < length; ++i) {
        KillEaterLevel* level = levels_.at( i );
        JUTIL::BaseAllocator::destroy( level );
    }
    levels_.clear();
}

bool KillEaterRank::add_level( KillEaterLevel* level )
{
	return levels_.push( level );
}

size_t KillEaterRank::get_level_count( void ) const
{
	return levels_.get_length();
}

const KillEaterLevel* KillEaterRank::get_level( size_t index ) const
{
	return levels_.at( index );
}

const KillEaterLevel* KillEaterRank::find_level( uint32 score ) const
{
    size_t i;
    size_t length = get_level_count();
    for (i = 0; i < length; ++i) {
        const KillEaterLevel* level = get_level( i );
		if(score < level->get_score()){
			return level;
		}
	}

	// Not found, return the highest level
	return get_level( length - 1 );
}
