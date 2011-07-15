#include "slot_view.h"

// Slot layout attributes.
const unsigned int SLOT_PADDING					= 5;
const unsigned int SLOT_SPACING					= 5;
const unsigned int SLOT_WIDTH					= 70;
const unsigned int SLOT_HEIGHT					= 60;
const unsigned int ITEM_SIZE					= 60;

// Slot stroke attributes.
const unsigned int SLOT_STROKE_WIDTH			= 2;
const D3DCOLOR SLOT_STROKE_NORMAL_COLOUR		= D3DCOLOR_XRGB( 248, 212, 0 );

// Slot colour attributes.
const D3DCOLOR SLOT_NORMAL_COLOUR				= D3DCOLOR_XRGB( 60, 53, 46 );
const D3DCOLOR SLOT_SELECTED_COLOUR				= D3DCOLOR_XRGB( 90, 80, 72 );
const D3DCOLOR SLOT_STROKE_WHITE_COLOUR			= D3DCOLOR_XRGB( 255, 255, 255 );

// Slot display attributes.
const unsigned int DRAG_ALPHA					= 185;
const unsigned int SLOT_RADIUS					= 5;

// Class-wide components.
RoundedRectangle* SlotView::normalSlot_			= nullptr;
RoundedRectangle* SlotView::selectedSlot_		= nullptr;
RoundedRectangle* SlotView::stroke_				= nullptr;

// Class-wide font/text resources.
Font* SlotView::equippedFont_					= nullptr;
Text* SlotView::equippedText_					= nullptr;

// Equipped text attributes.
const char* EQUIPPED_FONT_FACE					= "TF2 Build";
const unsigned int EQUIPPED_FONT_SIZE			= 10;
const bool EQUIPPED_FONT_BOLDED					= false;
const float EQUIPPED_PADDING					= 5;

SlotView::SlotView( Slot* slot )
{
	slot_ = slot;
	SetSelected( false );

	// Image of slot.
	slotImage_ = new Image( normalSlot_->GetTexture() );
	slotImage_->SetSize( SLOT_WIDTH, SLOT_HEIGHT );
	Add( slotImage_ );
	SetConstraint( slotImage_, 0.0f, 0.0f );

	// Image of item.
	itemImage_ = new Image( nullptr );
	itemImage_->SetSize( ITEM_SIZE, ITEM_SIZE );
	Add( itemImage_ );
	SetConstraint( itemImage_, (SLOT_WIDTH - ITEM_SIZE) / 2.0f, (SLOT_HEIGHT - ITEM_SIZE) / 2.0f );

	// Image of quality stroke.
	strokeImage_ = new Image( nullptr );
	strokeImage_->SetSize( SLOT_WIDTH, SLOT_HEIGHT );
	Add( strokeImage_ );
	SetConstraint( strokeImage_, 0.0f, 0.0f );

	SetSize( SLOT_WIDTH, SLOT_HEIGHT );
}

void SlotView::Update()
{
	// Update slot image.
	slotImage_->SetTexture( (IsSelected() ? selectedSlot_ : normalSlot_)->GetTexture() );

	// Get stroke and item texture.
	Texture* itemTexture = nullptr;
	if (slot_->HasItem()) {
		Item* item = slot_->GetItem();
		itemTexture = item->GetTexture();
		strokeImage_->SetTexture( stroke_->GetTexture() );
		strokeImage_->SetTint( item->GetQualityColour() );
	}
	else {
		strokeImage_->SetTexture( nullptr );
	}
	
	itemImage_->SetTexture( itemTexture );
}

void SlotView::Draw( DirectX* directX )
{
	Update();
	Container::Draw( directX );

	// Draw equipped text.
	if (slot_->HasItem()) {
		Item* item = slot_->GetItem();
		if (item->IsEquipped()) {
			equippedText_->SetPosition( 
				GetX() + GetWidth() - equippedText_->GetWidth() - EQUIPPED_PADDING, 
				GetY() + GetHeight() - equippedText_->GetHeight() - EQUIPPED_PADDING );
			equippedText_->Draw( directX );
		}
	}
}

Slot* SlotView::GetSlot() const
{
	return slot_;
}

void SlotView::SetSelected( bool isSelected )
{
	isSelected_ = isSelected;
}

bool SlotView::IsSelected() const
{
	return isSelected_;
}

void SlotView::Precache( DirectX* directX )
{
	// Create rounded rectangles.
	normalSlot_		= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	selectedSlot_	= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_SELECTED_COLOUR );
	stroke_			= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, 0 );
	stroke_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_WHITE_COLOUR );
	stroke_->SetStrokeType( STROKE_TYPE_INNER );

	// Generate all.
	normalSlot_->Generate( directX );
	selectedSlot_->Generate( directX );
	stroke_->Generate( directX );

	// Create equipped font.
	equippedFont_ = directX->CreateFont( EQUIPPED_FONT_FACE, EQUIPPED_FONT_SIZE, EQUIPPED_FONT_BOLDED );
	equippedText_ = new Text( equippedFont_ );
	equippedText_->SetText( "EQUIPPED" );
	equippedText_->Pack();
}

void SlotView::Release()
{
	if (normalSlot_ != nullptr) {
		delete normalSlot_;
		normalSlot_ = nullptr;
	}

	if (selectedSlot_ != nullptr) {
		delete selectedSlot_;
		selectedSlot_ = nullptr;
	}

	if (stroke_ != nullptr) {
		delete stroke_;
		stroke_ = nullptr;
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