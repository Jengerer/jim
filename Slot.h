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

class Slot: public Draggable
{
public:
	Slot( int index = -1, Item* item = NULL );
	virtual ~Slot();

	// Drawing function.
	virtual void	draw( DirectX* directX );
	virtual void	setPosition( float x, float y );
	virtual void	updatePosition();
	virtual int		getWidth() const;
	virtual int		getHeight() const;

	// Slot attribute functions.
	Item*		getItem();
	void		setItem( Item* item );
	int			getIndex() const;
	void		setIndex( int index );
	void		setGroup( ESlotGroup group );
	ESlotGroup	getGroup() const;

	// Selection handling.
	ESelectType	getSelectType() const;
	void		setSelectType( ESelectType selectType );

	// Class-wide texture.
	static Texture* texture;

	// Input handling.
	virtual bool Slot::mouseEvent( Mouse* mouse, EMouseEvent eventType );

private:
	Item*		item_;
	int			index_;
	ESlotGroup	group_;
	D3DCOLOR	colour_;

	bool		isActive_;
	ESelectType	selectType_;
};