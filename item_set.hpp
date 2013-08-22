#ifndef ITEM_SET_HPP
#define ITEM_SET_HPP

#include "item.hpp"
#include <containers/vector.hpp>

/*
 * Class for handling the global pool of items in an inventory.
 * Set takes ownership of the items, so they are to be destroyed if not
 * handled by the item set anymore.
 */
class ItemSet
{

public:

	ItemSet( void );
	~ItemSet( void );

	// Item management functions.
	bool add( Item* item );
	void destroy( Item* item );
	Item* find( uint64 unique_id );
	void clear( void );

    // Item iteration functions.
    unsigned int get_size( void ) const;
    Item* get_item( unsigned int index );

private:

	JUTIL::Vector<Item*> items_;

};

#endif // ITEM_SET_HPP