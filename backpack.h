#ifndef BACKPACK_H
#define BACKPACK_H

#include "animated_grid_pages.h"
#include "inventory.h"
#include "slot_grid_pages.h"
#include "slot_grid_view.h"

class Backpack: public Inventory
{

public:

	Backpack( unsigned int excludedSlots );

	// Has backpack been loaded yet?
	bool	is_loaded( void ) const;
	void	set_loaded( bool isLoaded );

private:

	bool is_loaded_;

};

#endif // BACKPACK_H