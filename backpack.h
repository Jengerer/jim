#pragma once

#include <json/json.h>
#include <string>

#include "Container.h"
#include "GridLayout.h"
#include "HorizontalLayout.h"
#include "Inventory.h"
#include "IPrecachable.h"
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

// TODO: Make Steam singleton or a passed reference.
class Backpack: public Container, public Steam, public IMouseHandler, public IPrecachable
{
public:
	Backpack( float x, float y, Container* parent );
	virtual ~Backpack();

	// Container functions.
	void			Pack( void );

	// Precaching DirectX resources.
	void			Precache( DirectX *directX );
	void			Release( void );

	// Resource and interface management.
	virtual void	LoadInterfaces( void );
	virtual void	CloseInterfaces( void );

	// Steam message handling.
	void			HandleCallback( int id, void *callback );
	void			HandleMessage( int id, void *message, uint32 size );

	// Item actions.
	void			CraftSelected( void );
	void			UpdateButtons( void );

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
	void			EquipItem( Item *item, uint32 classFlags );
	void			UnequipItems( uint32 equipClass, EItemSlot slot );

	// Selection handling.
	void			SelectSlot( Slot *slot, ESelectType selectType );
	slotVector*		GetSelected( void );
	void			DeselectSlot( Slot *slot );
	void			DeselectAll( void );
	void			SetSelectMode( ESelectMode selectMode );

	// Item hovering handler.
	bool			IsHovering( void ) const;
	const Slot*		GetHovering( void ) const;

	// Mouse handling.
	virtual bool	OnMouseMoved( Mouse *mouse );
	virtual bool	OnLeftClicked( Mouse *mouse );
	virtual bool	OnLeftReleased( Mouse *mouse );

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

	// Inventory to display.
	Inventory			*inventory_;

	// Top level layouts.
	VerticalLayout		*backpackLayout_;
	HorizontalLayout	*buttonLayout_;
	HorizontalLayout	*pages_;
	HorizontalLayout	*excluded_;

	// UI buttons.
	LabelButton			*equipButton_;
	LabelButton			*craftButton_;
	LabelButton			*sortButton_;

	// Queue to direct notifications to.
	NotificationQueue	*notifications_;

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
	float				cameraSpeed_, cameraDest_;

};