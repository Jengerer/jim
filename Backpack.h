#pragma once

#include "Menu.h"
#include "Steam.h"
#include "Inventory.h"
#include "Container.h"
#include "ItemDisplay.h"
#include "VerticalLayout.h"
#include "HorizontalLayout.h"
#include "SerializedBuffer.h"

#define BACKPACK_PADDING		25
#define BACKPACK_PADDING_TOP	50

#define SLOT_SPACING			5
#define PAGE_SPACING			BACKPACK_PADDING * 2

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

	virtual void openInterfaces();
	virtual void closeInterfaces();

	// Removing.
	void removeSlots();

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
	void handleCamera();
	void updateTarget();

	// Inventory handling.
	bool			isLoaded() const;
	void			setLoaded();
	Slot*			insert( Item* item );
	virtual void	move( Slot *source, Slot *destination );

	virtual void	removeItem( uint64 uniqueId );
	void			updateItem( Item *item );
	void			equipItem( Item *item, const string& className );
	void			unequipItems( EClassEquip equipClass, const string& slotName );
	void			craftSelected();

	// Selection handling.
	void			select( Slot *slot, ESelectType selectType );
	slotVector*		getSelected();
	Slot*			getHovered();
	void			deselect( Slot *slot );
	void			deselectAll();
	void			setSelectMode( ESelectMode selectMode );

private:
	HorizontalLayout *columns_;

	// Selection variables.
	Slot *dragged_;
	Slot *hovered_;
	slotVector selected_;
	ESelectMode selectMode_;

	// Is backpack loaded yet?
	bool isLoaded_;

	// Display for item information.
	ItemDisplay *itemDisplay_;

	// Backpack navigation.
	int page_, excludedPage_;
	int pageDelay_;
	float cameraX_, cameraSpeed_, cameraDest_;
};