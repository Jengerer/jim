#include "Slot.h"

// Initialize the texture.
Texture* Slot::texture = 0;

// Colours for slot.
const D3DCOLOR SELECTED_COLOUR	= D3DCOLOR_ARGB( 255, 255, 255, 255 );
const D3DCOLOR HOVER_COLOUR		= D3DCOLOR_ARGB( 255, 200, 200, 200 );
const D3DCOLOR NORMAL_COLOUR	= D3DCOLOR_ARGB( 255, 100, 100, 100 );
const D3DCOLOR DRAG_COLOUR		= D3DCOLOR_ARGB( 175, 255, 255, 255 );

Slot::Slot( int index, Item* item )
{
	setItem( item );
	setIndex( index );

	// Inactive and deselected by default.
	isActive_ = false;
	setSelectType( SELECT_TYPE_NONE );
}

Slot::~Slot()
{
	// Slot is destroyed.
}

void Slot::draw( DirectX* directX )
{
	// Update slot position.
	updatePosition();

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

	// Draw slot texture.
	directX->drawTexture( texture, getX(), getY(), drawColour );

	Item* item = getItem();
	if (item) {
		// Draw it.
		item->draw( directX );
	}
}

ESlotGroup Slot::getGroup() const
{
	return group_;
}

void Slot::setGroup( ESlotGroup group )
{
	group_ = group;
}

Item* Slot::getItem()
{
	return item_;
}

void Slot::setItem( Item* item )
{
	item_ = item;
	if (item) {
		item->move( getIndex() + 1 );
		updatePosition();
	}
}

int Slot::getIndex() const
{
	return index_;
}

void Slot::setIndex( int index )
{
	index_ = index;
}

void Slot::setPosition( float x, float y )
{
	Component::setPosition( x, y );
	updatePosition();
}

void Slot::updatePosition()
{
	// Move the item.
	Item* item = getItem();
	if (item) {
		float itemX = getX() + (getWidth() / 2) - (item->getWidth() / 2);
		float itemY = getY() + (getHeight() / 2) - (item->getHeight() / 2);
		item->setPosition( itemX, itemY );
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

bool Slot::mouseEvent( Mouse* mouse, EMouseEvent eventType ) {
	if (Component::mouseEvent( mouse, eventType )) {
		return true;
	}

	return false;
}