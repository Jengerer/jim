#include "Slot.h"

// Colours for slot.
const D3DCOLOR SELECTED_COLOUR	= D3DCOLOR_ARGB( 125,	60,		53,		46 );
const D3DCOLOR HOVER_COLOUR		= D3DCOLOR_ARGB( 255,	200,	200,	200 );
const D3DCOLOR EMPTY_COLOUR		= D3DCOLOR_ARGB( 255,	60,		53,		46 );
const D3DCOLOR NORMAL_COLOUR	= D3DCOLOR_ARGB( 255,	60,		53,		46 );
const D3DCOLOR DRAG_COLOUR		= D3DCOLOR_ARGB( 100,	60,		53,		46 );

Slot::Slot( int index, Item* item )
{
	setItem( item );
	setIndex( index );
	setSize( SLOT_WIDTH, SLOT_HEIGHT );

	// Inactive and deselected by default.
	isActive_ = false;
	colour_ = NORMAL_COLOUR;
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

	// Draw stroke based on quality.
	Item *item = getItem();
	if (item != 0) {
		// Draw stroke based on quality.
		D3DCOLOR strokeColour;
		switch (item->getQuality()) {
		case EItemQuality::k_EItemQuality_Unique:
			strokeColour = SLOT_STROKE_VINTAGE;
			break;
		default:
			strokeColour = SLOT_STROKE_NORMAL;
			break;
		}

		// Draw stroked slot.
		directX->drawRoundedRect( getX(), getY(), SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, strokeColour );
		directX->drawRoundedRect( getX() + SLOT_STROKE_WIDTH, getY() + SLOT_STROKE_WIDTH, SLOT_WIDTH - SLOT_STROKE_WIDTH*2, SLOT_HEIGHT - SLOT_STROKE_WIDTH*2, SLOT_RADIUS - SLOT_STROKE_WIDTH, colour_ );
		item->draw( directX );
	}
	else {
		// Draw slot texture.
		directX->drawRoundedRect( getX(), getY(), SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, EMPTY_COLOUR );
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
	if (item != nullptr) {
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

	switch (selectType) {
	case SELECT_TYPE_DRAG:
		colour_ = DRAG_COLOUR;
		break;
	case SELECT_TYPE_NORMAL:
		colour_ = SELECTED_COLOUR;
		break;
	default:
		colour_ = NORMAL_COLOUR;
		break;
	}
}