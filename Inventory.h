#pragma once

#include "Steam.h"
#include "Item.h"
#include "Slot.h"
#include "SerializedBuffer.h"
#include <vector>

class Inventory: public Steam, public Drawable
{
public:
	Inventory(
		Window* window,
		float x, float y, 
		int width, int height, 
		int pages );
	~Inventory();

	// Initializing and closing.
	void			openInterfaces();
	void			closeInterfaces();

	// Drawable functions.
	void			draw( DirectX* directX );
	int				getWidth() const;
	int				getHeight() const;
	void			onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent );

	// Dimension getters.
	int				getCapacity() const;

	// Slot handling.
	void			createSlots();
	void			clearSlots();
	void			select( Slot* slot );
	void			move( Slot* slot1, Slot* slot2 );
	bool			isValid( uint8 index );
	Slot*			get( uint8 index );

	// State methods.
	void			setLoaded();
	bool			isLoaded() const;

	// Item handling.
	void			add( Item* item );
	void			remove( Item* item );
	void			clearItems();

	// Item selection methods.
	void			deleteSelected();
	void			craftSelected();

	// Steam callback handling.
	void			handleCallbacks();

private:
	// Vector for all items.
	vector<Item*>	items_;

	// Slot vectors.
	vector<Slot*>	inventory_;
	vector<Slot*>	excluded_;
	vector<Slot*>	selected_;

	// Window for overflow width.
	Window*			window_;

	// Inventory attributes.
	bool			isLoaded_;
	int				width_, height_;
	int				pages_;
};
