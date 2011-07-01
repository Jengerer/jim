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
const D3DCOLOR SLOT_STROKE_VINTAGE_COLOUR		= D3DCOLOR_XRGB( 69, 97, 141 );
const D3DCOLOR SLOT_STROKE_GENUINE_COLOUR		= D3DCOLOR_XRGB( 75, 115, 83 );

// Slot colour attributes.
const D3DCOLOR SLOT_SELECTED_COLOUR				= D3DCOLOR_XRGB( 108, 96, 83 );
const D3DCOLOR SLOT_NORMAL_COLOUR				= D3DCOLOR_XRGB( 60, 53, 46 );

// Slot display attributes.
const unsigned int DRAG_ALPHA					= 185;
const unsigned int SLOT_RADIUS					= 5;

// Class-wide components.
RoundedRectangle* SlotView::emptySlot_			= nullptr;
RoundedRectangle* SlotView::normalSlot_			= nullptr;
RoundedRectangle* SlotView::normalSelected_		= nullptr;
RoundedRectangle* SlotView::vintageSlot_		= nullptr;
RoundedRectangle* SlotView::vintageSelected_	= nullptr;
RoundedRectangle* SlotView::genuineSlot_		= nullptr;
RoundedRectangle* SlotView::genuineSelected_	= nullptr;

// Class-wide font/text resources.
Font* SlotView::equippedFont_					= nullptr;
Text* SlotView::equippedText_					= nullptr;

// Equipped text attributes.
const char* EQUIPPED_FONT_FACE					= "TF2 Build";
const unsigned int EQUIPPED_FONT_SIZE			= 10;
const bool EQUIPPED_FONT_BOLDED					= false;

SlotView::SlotView( Slot* slot )
{
	slot_ = slot;
	SetSelected( false );

	// Add images.
	slotImage_ = new Image( emptySlot_->GetTexture(), 0.0f, 0.0f );
	slotImage_->SetSize( SLOT_WIDTH, SLOT_HEIGHT );
	itemImage_ = new Image( nullptr, (SLOT_WIDTH - ITEM_SIZE) / 2.0f, 0.0f );
	itemImage_->SetSize( ITEM_SIZE, ITEM_SIZE );
	Add( slotImage_ );
	Add( itemImage_ );
	SetSize( SLOT_WIDTH, SLOT_HEIGHT );
}

void SlotView::Update()
{
	// Get rounded rectangle.
	RoundedRectangle* whichRect = emptySlot_;
	Texture* itemTexture = nullptr;
	if (slot_->HasItem()) {
		Item* item = slot_->GetItem();
		itemTexture = item->GetTexture();

		bool isSelected = IsSelected();
		switch(item->GetQuality()) {
		case EItemQuality::k_EItemQuality_Unique:
			whichRect = isSelected ? vintageSelected_ : vintageSlot_;
			break;

		case EItemQuality::k_EItemQuality_Common:
			whichRect = isSelected ? vintageSelected_ : vintageSlot_;
			break;

		default:
			whichRect = isSelected ? normalSelected_ : normalSlot_;
			break;
		}
	}

	itemImage_->SetTexture( itemTexture );
	slotImage_->SetTexture( whichRect->GetTexture() );
}

void SlotView::OnDraw( DirectX* directX )
{
	Update();
	Container::OnDraw( directX );
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
	emptySlot_			= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );

	normalSlot_			= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	normalSelected_		= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_SELECTED_COLOUR );

	vintageSlot_		= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	vintageSelected_	= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_SELECTED_COLOUR );

	genuineSlot_		= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	genuineSelected_	= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_SELECTED_COLOUR );

	// Set up attributes.
	normalSlot_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_NORMAL_COLOUR );
	normalSlot_->SetStrokeType( STROKE_TYPE_INNER );
	normalSelected_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_NORMAL_COLOUR );
	normalSelected_->SetStrokeType( STROKE_TYPE_INNER );

	vintageSlot_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_VINTAGE_COLOUR );
	vintageSlot_->SetStrokeType( STROKE_TYPE_INNER );
	vintageSelected_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_VINTAGE_COLOUR );
	vintageSelected_->SetStrokeType( STROKE_TYPE_INNER );

	genuineSlot_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_GENUINE_COLOUR );
	genuineSlot_->SetStrokeType( STROKE_TYPE_INNER );
	genuineSelected_->SetStroke( SLOT_STROKE_WIDTH, SLOT_STROKE_GENUINE_COLOUR );
	genuineSelected_->SetStrokeType( STROKE_TYPE_INNER );

	// Generate all rectangles.
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

void SlotView::Release()
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