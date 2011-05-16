#include "Slot.h"

#define SLOT_RADIUS 5

#define SLOT_STROKE_WIDTH			1
#define SLOT_STROKE_NORMAL_COLOUR	D3DCOLOR_XRGB( 248, 212, 0 )
#define SLOT_STROKE_VINTAGE_COLOUR	D3DCOLOR_XRGB( 69, 97, 141 )

#define SLOT_SELECTED_COLOUR		D3DCOLOR_XRGB( 108, 96, 83 )
#define SLOT_NORMAL_COLOUR			D3DCOLOR_XRGB( 60, 53, 46 )

#define ITEM_SIZE 60

RoundedRectangle	*Slot::emptySlot_		= nullptr;
RoundedRectangle	*Slot::normalSlot_		= nullptr;
RoundedRectangle	*Slot::normalSelected_	= nullptr;
RoundedRectangle	*Slot::vintageSlot_		= nullptr;
RoundedRectangle	*Slot::vintageSelected_	= nullptr;

//=============================================================
// Constructor
//=============================================================
Slot::Slot( int index, Item* item )
{
	// Create image.
	image_ = new Image( 0, 0 );
	image_->SetSize( ITEM_SIZE, ITEM_SIZE );
	Add( image_ );

	SetItem( item );
	SetIndex( index );
	SetSize( SLOT_WIDTH, SLOT_HEIGHT );

	// Inactive and deselected by default.
	isActive_ = false;
	SetSelectType( SELECT_TYPE_NONE );
}

Slot::~Slot()
{
	// Slot is destroyed.
}

void Slot::OnDraw( DirectX* directX )
{
	// Draw stroke based on quality.
	Item *item				= GetItem();
	RoundedRectangle* whichRect = emptySlot_;

	// Alter texture and tint based on attributes.
	if (item != nullptr) {
		if ( GetSelectType() == SELECT_TYPE_NONE ) {
			switch ( item->GetQuality() ) {
			case EItemQuality::k_EItemQuality_Unique:
				whichRect = vintageSlot_;
				break;
			default:
				whichRect = normalSlot_;
				break;
			}
		}
		else {
			switch ( item->GetQuality() ) {
			case EItemQuality::k_EItemQuality_Unique:
				whichRect = vintageSelected_;
				break;
			default:
				whichRect = normalSelected_;
				break;
			}
		}
	}

	directX->DrawTexture( whichRect->GetTexture(), GetX(), GetY(), SLOT_WIDTH, SLOT_HEIGHT, D3DCOLOR_RGBA( 255, 255, 255, GetAlpha() ) );
	image_->OnDraw( directX );
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
		// Update image's texture.
		image_->SetTexture( item->GetTexture() );

		// Retain flags if this is an excluded slot.
		if (GetGroup() == GROUP_INVENTORY) {
			item->SetIndex( GetIndex() );
		}

		UpdatePosition();
	}
	else {
		image_->SetTexture( nullptr );
	}
}

//=============================================================
// Purpose: Gets the slot of the index, starts at 0.
//=============================================================
int Slot::GetIndex( void ) const
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
		float itemX = GetX() + (GetWidth() / 2) - (image_->GetWidth() / 2);
		float itemY = GetY() + (GetHeight() / 2) - (image_->GetHeight() / 2);
		image_->SetPosition( itemX, itemY );
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
		SetAlpha( 200 );
		break;
	default:
		SetAlpha( 255 );
		break;
	}
}

void Slot::Precache( DirectX *directX )
{
	emptySlot_			= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	normalSlot_			= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	normalSelected_		= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_SELECTED_COLOUR );
	vintageSlot_		= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	vintageSelected_	= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_SELECTED_COLOUR );

	// Set up attributes.
	normalSlot_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_NORMAL_COLOUR );
	normalSelected_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_NORMAL_COLOUR );
	vintageSlot_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_VINTAGE_COLOUR );
	vintageSelected_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_VINTAGE_COLOUR );

	// Generate them.
	emptySlot_->Generate( directX );
	normalSlot_->Generate( directX );
	normalSelected_->Generate( directX );
	vintageSlot_->Generate( directX );
	vintageSelected_->Generate( directX );
}

void Slot::Release( void )
{
	if (emptySlot_ != nullptr) {
		delete emptySlot_;
		emptySlot_ = nullptr;
	}

	if (normalSlot_ != nullptr) {
		delete normalSlot_;
		emptySlot_ = nullptr;
	}

	if (normalSelected_ != nullptr) {
		delete normalSelected_;
		normalSelected_ = nullptr;
	}

	if (vintageSlot_ != nullptr) {
		delete vintageSlot_;
		vintageSlot_ = nullptr;
	}

	if (vintageSelected_ != nullptr) {
		delete vintageSelected_;
		vintageSelected_ = nullptr;
	}
}