#ifndef ITEM_SET_HPP
#define ITEM_SET_HPP

#include "item.hpp"
#include <containers/vector.hpp>

/*
 * Class for handling the global pool of items in an inventory.
 */
class ItemSet
{

public:

	ItemSet( void );
	~ItemSet( void );

	// Item management functions.
	bool add_item( Item* item );
	void delete_item( Item* item );
	Item* get_item( uint64 unique_id );
	void clear( void );

private:

	JUTIL::Vector<Item*> items_;

};

#endif // ITEM_SET_HPP