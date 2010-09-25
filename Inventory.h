#pragma once

#include "Steam.h"
#include "Item.h"
#include "Slot.h"
#include <vector>

class Inventory: public Steam, public Drawable
{
public:
	Inventory(int width, int height, int pages);
	~Inventory();

	/* Initializing and ending. */
	void					loadInterfaces();
	void					closeInterfaces();

	// Slot handling.
	void					createSlots();
	void					clearSlots();
	void					select( Slot* slot );
	void					move( Slot* slot1, Slot* slot2 );
	bool					isValid( uint8 index );
	Slot*					get( uint8 index );
	const vector<Slot*>*	getSlots();

	// State methods.
	void					setLoaded();
	bool					isLoaded() const;

	// Item handling.
	void					add( Item* item );
	void					remove( Item* item );
	void					clearItems();

	// Item selection methods.
	void					deleteSelected();
	void					craftSelected();

	// Steam callback handling.
	void					handleCallbacks();

private:
	// Vector for all items.
	vector<Item*>	items_;

	// Slot vectors.
	vector<Slot*>	inventory_;
	vector<Slot*>	excluded_;
	vector<Slot*>	selected_;

	// Inventory attributes.
	bool			isLoaded_;
	int				width_, height_;
	int				pages_;
};
