#include "kill_eater_level.hpp"

/*
 * Constructor for storing item information by item parameters.
 */
KillEaterLevel::KillEaterLevel(uint32 score, JUTIL::String* prefix ) :
	score_( score ),
	prefix_( prefix )
{
}

/*
 * Item information destructor.
 */
KillEaterLevel::~KillEaterLevel( void )
{
	JUTIL::BaseAllocator::destroy( prefix_ );
}

uint32 KillEaterLevel::get_score( void ) const
{
	return score_;
}

const JUTIL::String* KillEaterLevel::get_prefix( void ) const
{
	return prefix_;
}

void KillEaterLevel::set_score( uint32 score )
{
	score_ = score;
}

void KillEaterLevel::set_prefix( JUTIL::String* prefix )
{
	prefix_ = prefix;
}
