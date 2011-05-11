#include "Slot.h"

// Colours for slot.
const D3DCOLOR SELECTED_COLOUR	= D3DCOLOR_ARGB( 125,	60,		53,		46 );
const D3DCOLOR HOVER_COLOUR		= D3DCOLOR_ARGB( 255,	200,	200,	200 );
const D3DCOLOR EMPTY_COLOUR		= D3DCOLOR_ARGB( 255,	60,		53,		46 );
const D3DCOLOR NORMAL_COLOUR	= D3DCOLOR_ARGB( 255,	60,		53,		46 );
const D3DCOLOR DRAG_COLOUR		= D3DCOLOR_ARGB( 100,	60,		53,		46 );

//=============================================================
// Constructor
//=============================================================
Slot::Slot( int index, Item* item )
{
	SetItem( item );
	SetIndex( index );
	SetSize( SLOT_WIDTH, SLOT_HEIGHT );

	// Inactive and deselected by default.
	isActive_ = false;
	colour_ = NORMAL_COLOUR;
	SetSelectType( SELECT_TYPE_NONE );
}

Slot::~Slot()
{
	// Slot is destroyed.
}

void Slot::OnDraw( DirectX* directX )
{
	// Draw stroke based on quality.
	Item *item = GetItem();
	if (item != nullptr) {
		// Draw stroke based on quality.
		D3DCOLOR strokeColour;
		switch ( item->GetQuality() ) {
		case EItemQuality::k_EItemQuality_Unique:
			strokeColour = SLOT_STROKE_VINTAGE;
			break;
		default:
			strokeColour = SLOT_STROKE_NORMAL;
			break;
		}

		// Draw stroked slot.
		directX->drawRoundedRect( GetX(), GetY(), SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, strokeColour );
		directX->drawRoundedRect( GetX() + SLOT_STROKE_WIDTH, GetY() + SLOT_STROKE_WIDTH, SLOT_WIDTH - SLOT_STROKE_WIDTH*2, SLOT_HEIGHT - SLOT_STROKE_WIDTH*2, SLOT_RADIUS - SLOT_STROKE_WIDTH, colour_ );
		item->OnDraw( directX );
	}
	else {
		// Draw slot texture.
		directX->drawRoundedRect( GetX(), GetY(), SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, EMPTY_COLOUR );
	}
}

ESlotGroup Slot::GetGroup() const
{
	return group_;
}

void Slot::SetGroup( ESlotGroup group )
{
	group_ = group;
}

bool Slot::HasItem( void ) const
{
	return item_ != nullptr;
}

Item* Slot::GetItem( void ) const
{
	return item_;
}

void Slot::SetItem( Item* item )
{
	item_ = item;
	if (item != nullptr) {
		item->SetIndex( GetIndex() );
		UpdatePosition();
	}
}

//=============================================================
// Purpose: Gets the slot of the index, starts at 0.
//=============================================================
int Slot::GetIndex() const
{
	return index_;
}

//=============================================================
// Purpose: Sets the slot of the index, starts at 0.
//=============================================================
void Slot::SetIndex( int index )
{
	index_ = index;
}

//=============================================================
// Updates item position when moved.
//=============================================================
void Slot::UpdatePosition( void )
{
	// Move the item.
	Item* item = GetItem();
	if (item != nullptr) {
		float itemX = GetX() + (GetWidth() / 2) - (item->GetWidth() / 2);
		float itemY = GetY() + (GetHeight() / 2) - (item->GetHeight() / 2);
		item->SetPosition( itemX, itemY );
	}
}

ESelectType Slot::GetSelectType( void ) const
{
	return selectType_;
}

void Slot::SetSelectType( ESelectType selectType )
{
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