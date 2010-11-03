#pragma once

#include "Texture.h"
#include "Draggable.h"
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

	// Drawing function.
	void		draw( DirectX* directX );
	int			getWidth() const;
	int			getHeight() const;

	// Selection handling.
	ESelectType	getSelectType() const;
	void		setSelectType( ESelectType selectType );

	// Input handling.
	void	onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent );

	// Class-wide texture.
	static Texture* texture;

	// The item stored by this slot.
	Item*	item;
	int		index;

private:
	bool	isActive_;
	ESelectType selectType_;
};