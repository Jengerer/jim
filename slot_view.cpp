#include "slot_view.h"

// Slot layout attributes.
const unsigned int SLOT_PADDING					= 5;
const unsigned int SLOT_SPACING					= 5;
const unsigned int SLOT_WIDTH					= 70;
const unsigned int SLOT_HEIGHT					= 60;
const unsigned int ITEM_SIZE					= 60;

// Slot stroke attributes.
const unsigned int SLOT_STROKE_WIDTH			= 2;
const Colour SLOT_STROKE_NORMAL_COLOUR( 248, 212, 0 );

// Slot colour attributes.
const Colour SLOT_NORMAL_COLOUR( 60, 53, 46 );
const Colour SLOT_SELECTED_COLOUR( 90, 80, 72 );

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
const char* EQUIPPED_FONT_FACE					= "fonts/tf2build.ttf";
const unsigned int EQUIPPED_FONT_SIZE			= 6;
const bool EQUIPPED_FONT_BOLDED					= false;
const float EQUIPPED_PADDING					= 5;

SlotView::SlotView( Slot* slot )
{
	slot_ = slot;
	set_selected( false );

	// Image of slot.
	slotImage_ = new Image( normalSlot_->get_texture() );
	slotImage_->set_size( SLOT_WIDTH, SLOT_HEIGHT );
	add( slotImage_ );
	set_constraint( slotImage_, 0.0f, 0.0f );

	// Image of item.
	itemImage_ = new Image( nullptr );
	itemImage_->set_size( ITEM_SIZE, ITEM_SIZE );
	add( itemImage_ );
	set_constraint( itemImage_, (SLOT_WIDTH - ITEM_SIZE) / 2.0f, (SLOT_HEIGHT - ITEM_SIZE) / 2.0f );

	// Image of quality stroke.
	strokeImage_ = new Image( nullptr );
	strokeImage_->set_size( SLOT_WIDTH, SLOT_HEIGHT );
	add( strokeImage_ );
	set_constraint( strokeImage_, 0.0f, 0.0f );

	set_size( SLOT_WIDTH, SLOT_HEIGHT );
}

#include <sstream>

void SlotView::Update()
{
	// Update slot image.
	slotImage_->set_texture( (is_selected() ? selectedSlot_ : normalSlot_)->get_texture() );

	// Get stroke and item texture.
	const Texture* itemTexture = nullptr;
	if (slot_->has_item()) {
		Item* item = slot_->get_item();
		itemTexture = item->get_texture();
		strokeImage_->set_texture( stroke_->get_texture() );
		strokeImage_->set_tint( item->get_qualityColour() );
	}
	else {
		strokeImage_->set_texture( nullptr );
	}
	
	itemImage_->set_texture( itemTexture );
}

void SlotView::draw( Graphics2D* graphics )
{
	Update();
	Container::draw( graphics );

	// Draw equipped text.
	if (slot_->has_item()) {
		Item* item = slot_->get_item();
		if (item->is_equipped()) {
			equippedText_->set_position( 
				get_x() + get_width() - equippedText_->get_width() - EQUIPPED_PADDING, 
				get_y() + get_height() - equippedText_->get_height() - EQUIPPED_PADDING );
			equippedText_->draw( graphics );
		}
	}
}

Slot* SlotView::get_slot() const
{
	return slot_;
}

void SlotView::set_selected( bool is_selected )
{
	is_selected_ = is_selected;
}

bool SlotView::is_selected() const
{
	return is_selected_;
}

void SlotView::precache( Graphics2D* graphics )
{
	// Create rounded rectangles.
	normalSlot_		= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_NORMAL_COLOUR );
	selectedSlot_	= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, SLOT_SELECTED_COLOUR );
	stroke_			= new RoundedRectangle( SLOT_WIDTH, SLOT_HEIGHT, SLOT_RADIUS, COLOUR_BLANK );
	stroke_->SetStroke( SLOT_STROKE_WIDTH, COLOUR_WHITE );
	stroke_->SetStrokeType( STROKE_TYPE_INNER );

	// Generate all.
	normalSlot_->Generate( graphics );
	selectedSlot_->Generate( graphics );
	stroke_->Generate( graphics );

	// Create equipped font.
	equippedFont_ = FontFactory::create_font( EQUIPPED_FONT_FACE, EQUIPPED_FONT_SIZE );
	equippedText_ = new Text( equippedFont_ );
	equippedText_->SetText( "EQUIPPED" );
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