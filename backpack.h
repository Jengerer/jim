#ifndef BACKPACK_H
#define BACKPACK_H

#include "animated_grid_pages.h"
#include "inventory.h"
#include "slot_grid_pages.h"
#include "slot_grid_view.h"

class Backpack: public Inventory
{

public:

	Backpack( unsigned int inventorySize, unsigned int excludedSlots );

	SlotGridPages*	CreateInventoryView( unsigned int width, unsigned int height ) const;
	SlotGridView*	CreateExcludedView() const;

	// Has backpack been loaded yet?
	bool	IsLoaded( void ) const;
	void	SetLoaded( bool isLoaded );

private:

	// Is backpack loaded yet?
	bool				isLoaded_;

};

#endif // BACKPACK_H