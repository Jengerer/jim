#include "kill_eater_type.hpp"

/*
 * Constructor for storing strange types.
 */
KillEaterType::KillEaterType( JUTIL::String* description, const KillEaterRanks* levels )
{
	set_description( description );
	set_levels( levels );
}

/*
 * Strange type destructor.
 */
KillEaterType::~KillEaterType( void )
{
    // Destroy name.
    JUTIL::BaseAllocator::destroy( description_ );
}

/*
 * Get handle to item description string.
 */
const JUTIL::String* KillEaterType::get_description( void ) const
{
	return description_;
}

/*
 * Get pointer to strange levels
 */
const KillEaterRanks* KillEaterType::get_levels( void ) const
{
	return levels_;
}

void KillEaterType::set_description( JUTIL::String* description )
{
	description_ = description;
}

void KillEaterType::set_levels( const KillEaterRanks* levels )
{
	levels_ = levels;
}
