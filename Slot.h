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

class Slot: public Draggable
{
public:
	Slot( Item* item = NULL, int index = -1 );
	virtual ~Slot();

	// Drawing function.
	virtual void	draw( DirectX* directX );
	virtual void	setPosition( float x, float y );
	virtual void	updatePosition();
	virtual int		getWidth() const;
	virtual int		getHeight() const;

	// Slot attribute functions.
	Item* getItem();
	void setItem( Item* item );
	int getIndex() const;

	// Selection handling.
	ESelectType	getSelectType() const;
	void		setSelectType( ESelectType selectType );

	// Class-wide texture.
	static Texture* texture;

	// The item stored by this slot.

private:
	Item*	item_;
	int		index_;

	bool	isActive_;
	ESelectType selectType_;
};