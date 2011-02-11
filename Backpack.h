#pragma once

#include "Inventory.h"
#include "Container.h"
#include "Steam.h"
#include "SerializedBuffer.h"
#include "ItemDisplay.h"

#define BACKPACK_PADDING		25
#define BACKPACK_PADDING_TOP	50

#define EXCLUDED_Y			415

enum ESelectMode {
	SELECT_MODE_SINGLE,
	SELECT_MODE_MULTIPLE,
};

class Backpack: public Inventory, public Container, public Steam
{
public:
	Backpack( float x, float y,
		int width, int height,
		int pages,
		Container* parent );
	virtual ~Backpack();

	// Removing.
	void removeSlots();

	// Drawing functions.
	void	draw( DirectX* directX );

	// Position handling.
	virtual void updatePosition();

	// Mouse handling.
	virtual bool mouseMoved( Mouse *mouse );
	virtual bool leftClicked( Mouse *mouse );
	virtual bool leftReleased( Mouse *mouse );

	// Slot interface handling functions.
	void slotClicked( Mouse *mouse, Slot *slot );
	void slotReleased( Slot *slot );

	// Page viewing functions.
	void nextPage();
	void prevPage();
	void moveCamera();

	// Inventory handling.
	bool			isLoaded() const;
	void			setLoaded();
	Slot*			insert( Item* item );
	virtual void	move( Slot *source, Slot *destination );
	virtual void	removeItem( uint64 uniqueId );
	void			updateItem( Item *item );
	void			craftSelected();

	// Selection handling.
	void			select( Slot *slot, ESelectType selectType );
	void			deselect( Slot *slot );
	void			deselectAll();
	void			setSelectMode( ESelectMode selectMode );

private:
	// Selection variables.
	Slot *dragged_;
	slotVector selected_;
	ESelectMode selectMode_;

	// Is backpack loaded yet?
	bool isLoaded_;

	// Display for item information.
	ItemDisplay *itemDisplay_;

	// Backpack navigation.
	int page_, excludedPage_;
	int pageDelay;
	float cameraX_, cameraDest_, cameraSpeed_;
};