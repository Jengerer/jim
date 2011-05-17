#include "Slot.h"

#define SLOT_PADDING 5
#define SLOT_RADIUS 5

#define SLOT_STROKE_WIDTH			2
#define SLOT_STROKE_NORMAL_COLOUR	D3DCOLOR_XRGB( 248, 212, 0 )
#define SLOT_STROKE_VINTAGE_COLOUR	D3DCOLOR_XRGB( 69, 97, 141 )
#define SLOT_STROKE_GENUINE_COLOUR	D3DCOLOR_XRGB( 75, 115, 83 )

#define SLOT_SELECTED_COLOUR		D3DCOLOR_XRGB( 108, 96, 83 )
#define SLOT_NORMAL_COLOUR			D3DCOLOR_XRGB( 60, 53, 46 )

#define EQUIPPED_FONT_FACE			"TF2 Build"
#define EQUIPPED_FONT_SIZE			10
#define EQUIPPED_FONT_BOLDED		false

#define ITEM_SIZE 60

RoundedRectangle	*Slot::emptySlot_		= nullptr;
RoundedRectangle	*Slot::normalSlot_		= nullptr;
RoundedRectangle	*Slot::normalSelected_	= nullptr;
RoundedRectangle	*Slot::vintageSlot_		= nullptr;
RoundedRectangle	*Slot::vintageSelected_	= nullptr;
RoundedRectangle	*Slot::genuineSlot_		= nullptr;
RoundedRectangle	*Slot::genuineSelected_	= nullptr;

Font *Slot::equippedFont_ = nullptr;
Text *Slot::equippedText_ = nullptr;

//=============================================================
// Constructor
//=============================================================
Slot::Slot( int index, Item* item )
{
	// Create slot image.
	slotImage_ = new Image( 0, 0, emptySlot_->GetTexture() );
	slotImage_->SetSize( SLOT_WIDTH, SLOT_HEIGHT );
	Add( slotImage_ );

	// Create item image.
	itemImage_ = new Image( 0, 0 );
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

void Slot::OnDraw( DirectX* directX )
{
	// Draw stroke based on quality.
	Container::OnDraw( directX );

	// See if we should draw equipped.
	if (HasItem() && GetItem()->IsEquipped()) {
		equippedText_->SetPosition( GetX() + SLOT_WIDTH - SLOT_PADDING - equippedText_->GetWidth(), GetY() + SLOT_HEIGHT - SLOT_PADDING - equippedText_->GetHeight() );
		equippedText_->OnDraw( directX );
	}
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
	if (item != nullptr) {
		UpdatePosition();

		// Only update index if inventory slot.
		if (GetGroup() == GROUP_INVENTORY) {
			item->SetIndex( GetIndex() );
		}
	}

	UpdateSlot();
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
	slotImage_->SetPosition( GetX(), GetY() );
	
	// Now center item image.
	float itemX = GetX() + ((GetWidth() - itemImage_->GetWidth()) >> 1);
	float itemY = GetY() + ((GetHeight() - itemImage_->GetWidth()) >> 1);
	itemImage_->SetPosition( itemX, itemY );
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

	UpdateSlot();
}

void Slot::Precache( DirectX *directX )
{
	emptySlot_			= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	normalSlot_			= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	normalSelected_		= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_SELECTED_COLOUR );
	vintageSlot_		= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	vintageSelected_	= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_SELECTED_COLOUR );
	genuineSlot_		= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	genuineSelected_	= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_SELECTED_COLOUR );

	// Set up attributes.
	normalSlot_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_NORMAL_COLOUR );
	normalSelected_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_NORMAL_COLOUR );
	vintageSlot_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_VINTAGE_COLOUR );
	vintageSelected_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_VINTAGE_COLOUR );
	genuineSlot_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_GENUINE_COLOUR );
	genuineSelected_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_GENUINE_COLOUR );

	// Generate them.
	emptySlot_->Generate( directX );
	normalSlot_->Generate( directX );
	normalSelected_->Generate( directX );
	vintageSlot_->Generate( directX );
	vintageSelected_->Generate( directX );
	genuineSlot_->Generate( directX );
	genuineSelected_->Generate( directX );

	// Create equipped font.
	equippedFont_ = directX->CreateFont( EQUIPPED_FONT_FACE, EQUIPPED_FONT_SIZE, EQUIPPED_FONT_BOLDED );
	equippedText_ = new Text( equippedFont_ );
	equippedText_->SetText( "EQUIPPED" );
	equippedText_->Pack();
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

	if (genuineSlot_ != nullptr) {
		delete genuineSlot_;
		genuineSlot_ = nullptr;
	}

	if (genuineSelected_ != nullptr) {
		delete genuineSelected_;
		genuineSelected_ = nullptr;
	}

	if (equippedFont_ != nullptr) {
		delete equippedFont_;
		equippedFont_ = nullptr;
	}

	if (equippedText_ != nullptr) {
		delete equippedText_;
		equippedText_ = nullptr;
	}
}