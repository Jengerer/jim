#include "item_update_queue.hpp"

/*
 * Add an item to the queue if it's not there already.
 */
bool ItemUpdateQueue::add( const Item* item )
{
	uint64 id = item->get_unique_id();
	if (!queue_.contains( id )) {
		if (!queue_.insert( id, item )) {
			return false;
		}
	}
	return true;
}

/*
 * Remove an item if it's on the queue.
 * Returns true if the item was removed, false if it wasn't on queue.
 */
bool ItemUpdateQueue::remove( const Item* item )
{
	uint64 id = item->get_unique_id();
	if (queue_.contains( id )) {
		queue_.remove( id );
		return true;
	}
	return false;
}

/*
 * Returns number of items on the queue.
 */
unsigned int ItemUpdateQueue::get_queue_length( void ) const
{
	return queue_.get_length();
}

/*
 * Gets the next item to update.
 */
const Item* ItemUpdateQueue::get_next( void ) const
{
	JUTIL::Map<uint64, const Item*>::Iterator i = queue_.begin();
	return i.get_value();
}
