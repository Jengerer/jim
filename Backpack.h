#pragma once

#include <json/json.h>
#include <string>

#include "Container.h"
#include "HorizontalLayout.h"
#include "Inventory.h"
#include "Menu.h"
#include "NotificationQueue.h"
#include "SerializedBuffer.h"
#include "Steam.h"
#include "VerticalLayout.h"

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

	// Resource and interface management.
	virtual void	LoadInterfaces( void );
	virtual void	CloseInterfaces( void );

	// Steam message handling.
	void			HandleCallback( int id, void *callback );
	void			HandleMessage( int id, void *message, uint32 size );

	// Inventory resource handling.
	void			CreateInventory( int width, int height, int pages, int excludedSize );
	void			LoadInventory( const string& jsonInventory );
	void			FormatInventory( void );

	// Notification handling.
	void			SetNotificationQueue( NotificationQueue *notifications );

	// Has backpack been loaded yet?
	bool			IsLoaded( void ) const;
	void			SetLoaded( bool isLoaded );

	// Item handling.
	virtual void	MoveItem( Slot *source, Slot *destination );
	void			UpdateItem( Item *item );
	void			EquipItem( Item *item, const string& className );
	void			UnequipItems( EClassEquip equipClass, const string& slotName );
	void			CraftSelected( void );

	// Selection handling.
	void			SelectSlot( Slot *slot, ESelectType selectType );
	slotVector*		GetSelected( void );
	void			DeselectSlot( Slot *slot );
	void			DeselectAll( void );
	void			SetSelectMode( ESelectMode selectMode );

	// Item hovering handler.
	bool			IsHovering( void ) const;
	const Slot*		GetHovering( void ) const;

	// Position handling.
	virtual void	UpdatePosition( void );

	// Mouse handling.
	virtual bool	OnMouseMoved( Mouse *mouse );
	virtual bool	OnLeftClicked( Mouse *mouse );
	virtual bool	OnLeftReleased( Mouse *mouse );
	virtual bool	OnRightClicked( Mouse *mouse );
	virtual bool	OnRightReleased( Mouse *mouse );

	// Slot handling functions.
	void			RemoveSlots( void );
	void			OnSlotGrabbed( Mouse *mouse, Slot *slot );
	void			OnSlotReleased( Slot *slot );

	// Page viewing functions.
	void			NextPage( void );
	void			PrevPage( void );
	void			MoveCamera( void );
	void			HandleCamera( void );
	void			UpdateTarget( void );

private:

	void			SetHovering( const Slot* slot );

private:

	Inventory			*inventory_;

	// Queue to direct notifications to.
	NotificationQueue	*notifications_;

	// Layout objects.
	HorizontalLayout	*pages_;
	HorizontalLayout	*excluded_;

	// Selection variables.
	Slot				*dragged_;
	const Slot			*hovered_;
	slotVector			selected_;
	ESelectMode			selectMode_;

	// Is backpack loaded yet?
	bool				isLoaded_;

	// Backpack navigation.
	int					page_, excludedPage_;
	int					pageDelay_;
	float				cameraX_, cameraSpeed_, cameraDest_;

};