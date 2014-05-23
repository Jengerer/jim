#include "item_set.hpp"

ItemSet::ItemSet( void )
{
}

/*
 * Clear all items from the item set.
 */
ItemSet::~ItemSet( void )
{
    clear();
}

/*
 * Add an item to be managed by the item set.
 */
bool ItemSet::add( Item* item )
{
    if (!items_.push( item )) {
        return false;
    }
    return true;
}

/*
 * Remove and destroy an item handled by the set.
 */
void ItemSet::destroy( Item* item )
{
    bool removed = items_.remove( item );
    assert( removed );
}

/*
 * Find an item by unique ID in the set.
 * Returns nullptr if not found.
 */
Item* ItemSet::find( uint64 unique_id )
{
    unsigned int i;
    unsigned int end = get_size();
    for (i = 0; i < end; ++i) {
        Item* item = get_item( i );
        if (item->get_unique_id() == unique_id) {
            return item;
        }
    }
    return nullptr;
}

/*
 * Remove all items from the set.
 */
void ItemSet::clear( void )
{
    unsigned int i;
    unsigned int end = get_size();
    for (i = 0; i < end; ++i) {
        Item* item = get_item( i );
        JUTIL::BaseAllocator::destroy( item );
    }
    items_.clear();
}

/*
 * Get the number of items in the set.
 */
unsigned int ItemSet::get_size( void ) const
{
    return items_.get_length();
}

/*
 * Get an item at a specified index in the set.
 */
Item* ItemSet::get_item( unsigned int index )
{
    return items_.at( index );
}
