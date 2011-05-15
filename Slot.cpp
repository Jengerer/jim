#include "Slot.h"

#define SLOT_RADIUS 5

#define SLOT_STROKE_WIDTH			1
#define SLOT_STROKE_NORMAL_COLOUR	D3DCOLOR_XRGB( 248, 212, 0 )
#define SLOT_STROKE_VINTAGE_COLOUR	D3DCOLOR_XRGB( 69, 97, 141 )

#define SLOT_SELECTED_COLOUR		D3DCOLOR_XRGB( 108, 96, 83 )
#define SLOT_NORMAL_COLOUR			D3DCOLOR_XRGB( 60, 53, 46 )

#define ITEM_SIZE 60

Texture	*Slot::emptySlot_		= nullptr;
Texture	*Slot::normalSlot_		= nullptr;
Texture	*Slot::normalSelected_	= nullptr;
Texture	*Slot::vintageSlot_		= nullptr;
Texture	*Slot::vintageSelected_	= nullptr;

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
	Texture *slotTexture	= emptySlot_;

	// Alter texture and tint based on attributes.
	if (item != nullptr) {
		if ( GetSelectType() == SELECT_TYPE_NONE ) {
			switch ( item->GetQuality() ) {
			case EItemQuality::k_EItemQuality_Unique:
				slotTexture = vintageSlot_;
				break;
			default:
				slotTexture = normalSlot_;
				break;
			}
		}
		else {
			switch ( item->GetQuality() ) {
			case EItemQuality::k_EItemQuality_Unique:
				slotTexture = vintageSelected_;
				break;
			default:
				slotTexture = normalSelected_;
				break;
			}
		}
	}

	directX->DrawTexture( slotTexture, GetX(), GetY(), SLOT_WIDTH, SLOT_HEIGHT, D3DCOLOR_RGBA( 255, 255, 255, GetAlpha() ) );
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
	emptySlot_			= directX->CreateTexture( "empty_slot", SLOT_WIDTH, SLOT_HEIGHT );
	normalSlot_			= directX->CreateTexture( "normal_slot", SLOT_WIDTH, SLOT_HEIGHT );
	normalSelected_		= directX->CreateTexture( "normal_selected", SLOT_WIDTH, SLOT_HEIGHT );
	vintageSlot_		= directX->CreateTexture( "vintage_slot", SLOT_WIDTH, SLOT_HEIGHT );
	vintageSelected_	= directX->CreateTexture( "vintage_selected", SLOT_WIDTH, SLOT_HEIGHT );

	// Draw empty slot.
	directX->SetRenderTarget( emptySlot_ );
	directX->DrawRoundedRect( 0, 0, SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	directX->ResetRenderTarget();

	// Draw normal slot.
	directX->SetRenderTarget( normalSlot_ );
	directX->DrawRoundedRect( 0, 0, SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_STROKE_NORMAL_COLOUR );
	directX->DrawRoundedRect( SLOT_STROKE_WIDTH, SLOT_STROKE_WIDTH, SLOT_WIDTH - SLOT_STROKE_WIDTH*2, SLOT_HEIGHT - SLOT_STROKE_WIDTH*2, SLOT_RADIUS - SLOT_STROKE_WIDTH, SLOT_NORMAL_COLOUR );
	directX->ResetRenderTarget();

	// Draw normal selected.
	directX->SetRenderTarget( normalSelected_ );
	directX->DrawRoundedRect( 0, 0, SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_STROKE_NORMAL_COLOUR );
	directX->DrawRoundedRect( SLOT_STROKE_WIDTH, SLOT_STROKE_WIDTH, SLOT_WIDTH - SLOT_STROKE_WIDTH*2, SLOT_HEIGHT - SLOT_STROKE_WIDTH*2, SLOT_RADIUS - SLOT_STROKE_WIDTH, SLOT_SELECTED_COLOUR );
	directX->ResetRenderTarget();

	// Draw vintage slot.
	directX->SetRenderTarget( vintageSlot_ );
	directX->DrawRoundedRect( 0, 0, SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_STROKE_VINTAGE_COLOUR );
	directX->DrawRoundedRect( SLOT_STROKE_WIDTH, SLOT_STROKE_WIDTH, SLOT_WIDTH - SLOT_STROKE_WIDTH*2, SLOT_HEIGHT - SLOT_STROKE_WIDTH*2, SLOT_RADIUS - SLOT_STROKE_WIDTH, SLOT_NORMAL_COLOUR );
	directX->ResetRenderTarget();

	// Draw vintage selected.
	directX->SetRenderTarget( vintageSelected_ );
	directX->DrawRoundedRect( 0, 0, SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_STROKE_VINTAGE_COLOUR );
	directX->DrawRoundedRect( SLOT_STROKE_WIDTH, SLOT_STROKE_WIDTH, SLOT_WIDTH - SLOT_STROKE_WIDTH*2, SLOT_HEIGHT - SLOT_STROKE_WIDTH*2, SLOT_RADIUS - SLOT_STROKE_WIDTH, SLOT_SELECTED_COLOUR );
	directX->ResetRenderTarget();
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