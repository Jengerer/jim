#pragma once

#include "Texture.h"
#include "Drawable.h"
#include "Item.h"

// Slot selection type for manager handling.
enum ESelectType {
	SELECT_TYPE_NONE,
	SELECT_TYPE_NORMAL,
	SELECT_TYPE_DRAG
};

class Slot: public Drawable
{
public:
	Slot( Item* item = NULL );

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

private:
	bool	isActive_;
	ESelectType selectType_;
};