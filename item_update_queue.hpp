#ifndef ITEM_UPDATE_QUEUE_HPP
#define ITEM_UPDATE_QUEUE_HPP

#include "item.hpp"

/*
 * Manager for items to be updated.
 */
class ItemUpdateQueue
{

public:

	// Item queue management.
	bool add( const Item* item );
	bool remove( const Item* item );

	// Queue functions.
	unsigned int get_queue_length( void ) const;
	const Item* get_next( void ) const;

private:

	// Find items by unique ID.
	JUTIL::Map<uint64, const Item*> queue_;

};

#endif // ITEM_UPDATE_QUEUE_HPP