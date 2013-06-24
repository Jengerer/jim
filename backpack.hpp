#ifndef BACKPACK_H
#define BACKPACK_H

#include "inventory.hpp"
#include "slot_grid_view.hpp"

class Backpack: public Inventory
{

public:

	Backpack( SlotBook* book, DynamicSlotBook* excluded_book );

	// Has backpack been loaded yet?
	bool	is_loaded( void ) const;
	void	set_loaded( bool isLoaded );

private:

	bool is_loaded_;

};

#endif // BACKPACK_H