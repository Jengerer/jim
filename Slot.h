#pragma once

#include "Component.h"
#include "DirectX.h"
#include "Draggable.h"
#include "Image.h"
#include "Item.h"
#include "Texture.h"

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

class Slot : public Draggable
{

public:

	Slot( int index = 0, Item* item = nullptr );
	virtual ~Slot();

	// Drawing function.
	virtual void	OnDraw( DirectX* directX );
	virtual void	UpdatePosition();

	// Slot attribute functions.
	bool		HasItem( void ) const;
	Item*		GetItem( void ) const;
	void		SetItem( Item* item );
	int			GetIndex( void ) const;
	void		SetIndex( int index );
	void		SetGroup( ESlotGroup group );
	ESlotGroup	GetGroup( void ) const;

	// Selection handling.
	ESelectType	GetSelectType( void ) const;
	void		SetSelectType( ESelectType selectType );

	static void	Precache( DirectX *directX );
	static void Release( void );

protected:

	// TODO: Come up with a better set up, mixing together.
	static Texture	*emptySlot_;
	static Texture	*normalSlot_;
	static Texture	*normalSelected_;
	static Texture	*vintageSlot_;
	static Texture	*vintageSelected_;

private:

	Item*		item_;
	Image*		image_;
	int			index_;
	ESlotGroup	group_;

	bool		isActive_;
	ESelectType	selectType_;

};