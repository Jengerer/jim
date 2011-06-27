#include "slot.h"

//=============================================================
// Constructor
//=============================================================
Slot::Slot( int index, Item* item )
{
	// Create slot image.
	slotImage_ = new Image( emptySlot_->GetTexture() );
	slotImage_->SetSize( SLOT_WIDTH, SLOT_HEIGHT );
	Add( slotImage_ );

	// Create item image.
	itemImage_ = new Image( nullptr, (SLOT_WIDTH - ITEM_SIZE) / 2.0f, 0 );
	itemImage_->SetSize( ITEM_SIZE, ITEM_SIZE );
	Add( itemImage_ );

	// Set attributes.
	SetItem( item );
	SetIndex( index );
	SetSize( SLOT_WIDTH, SLOT_HEIGHT );

	// Deselected by default.
	SetSelectType( SELECT_TYPE_NONE );

	// Set drawing parameters.
	UpdateSlot();
}

Slot::~Slot()
{
	// Slot is destroyed.
}

void Slot::UpdateSlot( void )
{
	// Update item image.
	if (HasItem()) {
		Item *item = GetItem();
		itemImage_->SetTexture( item->GetTexture() );
	}
	else {
		itemImage_->SetTexture( nullptr );
	}

	// Update slot image.
	Texture *slotTexture = nullptr;
	if (HasItem()) {
		Item *item = GetItem();
		if ( GetSelectType() == SELECT_TYPE_NONE ) {
			switch ( item->GetQuality() ) {
			case EItemQuality::k_EItemQuality_Unique:
				slotTexture = vintageSlot_->GetTexture();
				break;
			case EItemQuality::k_EItemQuality_Common:
				slotTexture = genuineSlot_->GetTexture();
				break;
			default:
				slotTexture = normalSlot_->GetTexture();
				break;
			}
		}
		else {
			switch ( item->GetQuality() ) {
			case EItemQuality::k_EItemQuality_Unique:
				slotTexture = vintageSelected_->GetTexture();
				break;
			case EItemQuality::k_EItemQuality_Common:
				slotTexture = genuineSelected_->GetTexture();
				break;
			default:
				slotTexture = normalSelected_->GetTexture();
				break;
			}
		}
	}
	else {
		slotTexture = emptySlot_->GetTexture();
	}

	// Set the texture.
	slotImage_->SetTexture( slotTexture );
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