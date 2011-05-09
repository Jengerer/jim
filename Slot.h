#pragma once

#include "Texture.h"
#include "Draggable.h"
#include "Component.h"
#include "Item.h"

// Slot selection type for manager handling.
enum ESelectType {
	SELECT_TYPE_NONE,
	SELECT_TYPE_NORMAL,
	SELECT_TYPE_DRAG
};

// Slot type identifier.
enum ESlotGroup {
	GROUP_INVENTORY,
	GROUP_EXCLUDED
};

#define SLOT_WIDTH	70
#define SLOT_HEIGHT	60
#define SLOT_RADIUS 5

#define SLOT_STROKE_VINTAGE	D3DCOLOR_RGBA( 69, 97, 141, 200 )
#define SLOT_STROKE_NORMAL	D3DCOLOR_RGBA( 193, 165, 0, 150 )
#define SLOT_STROKE_WIDTH	2

class Slot : public Draggable
{

public:

	Slot( int index = 0, Item* item = nullptr );
	virtual ~Slot();

	// Drawing function.
	virtual void	OnDraw( DirectX* directX );
	virtual void	UpdatePosition();

	// Slot attribute functions.
	Item*		GetItem( void );
	void		SetItem( Item* item );
	int			GetIndex( void ) const;
	void		SetIndex( int index );
	void		SetGroup( ESlotGroup group );
	ESlotGroup	GetGroup( void ) const;

	// Selection handling.
	ESelectType	GetSelectType( void ) const;
	void		SetSelectType( ESelectType selectType );

private:

	Item*		item_;
	int			index_;
	ESlotGroup	group_;
	D3DCOLOR	colour_;

	bool		isActive_;
	ESelectType	selectType_;

};