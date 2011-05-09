#pragma once

#include <json/json.h>
#include <string>

#include "protobuf/steammessages.pb.h"
#include "protobuf/base_gcmessages.pb.h"

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

#define EXCLUDED_Y				415

enum ESelectMode {
	SELECT_MODE_SINGLE,
	SELECT_MODE_MULTIPLE,
};

class Backpack: public Container, public Steam, public IMouseHandler
{
public:
	Backpack( float x, float y, Container* parent );
	virtual ~Backpack();

	// Interface handling.
	void openInterfaces();
	void closeInterfaces();

	// Steam message handling.
	void handleCallback( int id, void *callback );
	void handleMessage( int id, void *message, uint32 size );

	// Inventory resource handling.
	void createInventory( int width, int height, int pages, int excludedSize );
	void loadInventory( const string& jsonInventory );
	void formatInventory();

	// Inventory attribute handling.
	bool isLoaded() const;
	void setLoaded( bool isLoaded );

	// Item handling.
	virtual void	moveItem( Slot *source, Slot *destination );
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

	// Position handling.
	virtual void updatePosition();

	// Mouse handling.
	virtual bool OnMouseMoved( Mouse *mouse );
	virtual bool OnLeftClicked( Mouse *mouse );
	virtual bool OnLeftReleased( Mouse *mouse );
	virtual bool OnRightClicked( Mouse *mouse );
	virtual bool OnRightReleased( Mouse *mouse );

	// Slot handling functions.
	void removeSlots();
	void slotGrabbed( Mouse *mouse, Slot *slot );
	void slotReleased( Slot *slot );

	// Page viewing functions.
	void nextPage();
	void prevPage();
	void moveCamera();
	void handleCamera();
	void updateTarget();

private:
	Inventory *inventory_;
	HorizontalLayout *pages_;
	HorizontalLayout *excluded_;

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