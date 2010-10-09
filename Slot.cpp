#include "Slot.h"

// Initialize the texture.
Texture* Slot::texture = 0;

// Colours for slot.
const D3DCOLOR SELECTED_COLOUR	= D3DCOLOR_ARGB( 255, 255, 255, 255 );
const D3DCOLOR HOVER_COLOUR		= D3DCOLOR_ARGB( 255, 200, 200, 200 );
const D3DCOLOR NORMAL_COLOUR	= D3DCOLOR_ARGB( 255, 100, 100, 100 );
const D3DCOLOR DRAG_COLOUR		= D3DCOLOR_ARGB( 175, 255, 255, 255 );

Slot::Slot( Item* item, int index )
{
	this->item = item;
	index_ = index;

	// Inactive and deselected by default.
	isActive_ = false;
	setSelectType( SELECT_TYPE_NONE );
}

void Slot::draw( DirectX* directX )
{
	// Check for collision.
	D3DCOLOR drawColour;
	switch (selectType_) {
	case SELECT_TYPE_NONE:
		drawColour = (isActive_ ? HOVER_COLOUR : NORMAL_COLOUR );
		break;
	case SELECT_TYPE_NORMAL:
		drawColour = SELECTED_COLOUR;
		break;
	case SELECT_TYPE_DRAG:
		drawColour = DRAG_COLOUR;
		break;
	}

	// Draw the slot texture here.
	directX->drawTexture( texture, x, y, drawColour );

	if (item) {
		// Move it to the center of this slot.
		item->x = x + (getWidth() / 2) - (item->getWidth() / 2);
		item->y = y + (getHeight() / 2) - (item->getHeight() / 2);

		// Draw it.
		item->draw( directX );
	}
}

void Slot::onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent )
{
	// Check collision.
	switch (mEvent) {
	case MOUSE_EVENT_MOVE:
		isActive_ = mouseTouching( mouseListener );
		break;
	case MOUSE_EVENT_RELEASE:
		setSelectType( SELECT_TYPE_NORMAL );
		break;
	}
}

ESelectType Slot::getSelectType() const {
	return selectType_;
}

void Slot::setSelectType( ESelectType selectType ) {
	selectType_ = selectType;
}

int Slot::getWidth() const
{
	return texture->getWidth();
}

int Slot::getHeight() const
{
	return texture->getHeight();
}