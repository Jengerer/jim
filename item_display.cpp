#include "item_display.h"
#include "font_factory.h"

const char* ITEM_DISPLAY_TITLE_FONT_FACE		= "fonts/tf2build.ttf";
const unsigned int ITEM_DISPLAY_TITLE_FONT_SIZE	= 13;

const Colour& ITEM_DISPLAY_COLOUR				= COLOUR_BLACK;

const char* ITEM_DISPLAY_INFO_FONT_FACE			= "fonts/tf2secondary.ttf";
const unsigned int ITEM_DISPLAY_INFO_FONT_SIZE	= 11;

Font *ItemDisplay::nameFont_ = nullptr;
Font *ItemDisplay::infoFont_ = nullptr;

const unsigned int ITEM_DISPLAY_PADDING	= 20;
const unsigned int ITEM_DISPLAY_SPACING	= 5;
const unsigned int ITEM_DISPLAY_RADIUS	= 5;
const int ITEM_DISPLAY_TEXT_WIDTH		= 200;
const int ITEM_DISPLAY_ALPHA_SPEED		= 30;
const int ITEM_DISPLAY_ALPHA_MAX		= 210;

ItemDisplay::ItemDisplay( void ) : RoundedRectangleContainer( ITEM_DISPLAY_RADIUS, ITEM_DISPLAY_PADDING )
{
	set_alpha( 0 );
	GetRoundedRectangle()->SetColour( ITEM_DISPLAY_COLOUR );
	set_item( nullptr );
	SetActive( false );

	// Create text objects.
	nameText_ = new WrappedText( nameFont_, ITEM_DISPLAY_TEXT_WIDTH );
	nameText_->set_text_formatting( DT_CENTER );
	infoText_ = new WrappedText( infoFont_, ITEM_DISPLAY_TEXT_WIDTH );
	infoText_->set_text_formatting( DT_CENTER );

	// Add to layout.
	textLayout_ = new VerticalLayout();
	textLayout_->SetSpacing( ITEM_DISPLAY_SPACING );
	textLayout_->add( nameText_ );
	textLayout_->add( infoText_ );

	// Pack so we can create a temporary rectangle.
	add( textLayout_ );
	SetContent( textLayout_ );
	pack();
}

ItemDisplay::~ItemDisplay( void )
{
}

void ItemDisplay::UpdateDisplay()
{
	// Alter display based on quality.
	nameText_->SetColour( item_->get_qualityColour() );
	SetName( item_->get_name() );

	// Build information text.
	std::stringstream infoStream;
	infoStream << "LEVEL " << static_cast<uint16>(item_->get_level());

#if defined( _DEBUG )
	infoStream << "\nFLAGS: " << std::hex << item_->get_flags();
	infoStream << "\nORIGIN: " << item_->get_origin();
	for (size_t i = 0; i < item_->get_attribute_count(); i++) {
		const Attribute* attrib = item_->get_attribute_at( i );
		if (attrib->has_description()) {
			infoStream << "\n" << attrib->get_description_string();
		}
	}
#endif

	// Get resulting string.
	const std::string& text = infoStream.str();

	// Get buffer of correct size.
	int wide_size = MultiByteToWideChar( CP_UTF8, 0, text.c_str(), text.length(), nullptr, 0 );
	wchar_t* wide_buffer = new wchar_t[ wide_size ];

	// Convert to wide string.
	if (wide_buffer != nullptr) {
		MultiByteToWideChar( CP_UTF8, 0, text.c_str(), text.length(), wide_buffer, wide_size );
		infoText_->SetText( wide_buffer, wide_size );

		// Delete wide string.
		delete wide_buffer;
	}

	pack();
}

void ItemDisplay::UpdateAlpha( void )
{
	if ( IsActive() ) {
		set_alpha( GetAlpha() + ITEM_DISPLAY_ALPHA_SPEED );
		if (GetAlpha() > ITEM_DISPLAY_ALPHA_MAX) {
			set_alpha( ITEM_DISPLAY_ALPHA_MAX );
		}
	}
	else {
		set_alpha( GetAlpha() - ITEM_DISPLAY_ALPHA_SPEED );
	}
}

void ItemDisplay::pack( void )
{
	textLayout_->pack();
	RoundedRectangleContainer::pack();
}

const Item* ItemDisplay::get_item( void ) const
{
	return item_;
}

void ItemDisplay::set_item( const Item *item )
{
	if (item_ != item) {
		item_ = item;
		if (item != nullptr) {
			SetActive( true );
			UpdateDisplay();
		}
		else {
			SetActive( false );
		}
	}
}

const std::string& ItemDisplay::get_name( void ) const
{
	return itemName_;
}

void ItemDisplay::SetName( const std::string& name )
{
	itemName_ = name;
	nameText_->SetText( name );
}

bool ItemDisplay::IsActive( void ) const
{
	return is_active_;
}

void ItemDisplay::SetActive( bool is_active )
{
	is_active_ = is_active;
}

void ItemDisplay::precache()
{
	nameFont_ = FontFactory::create_font( 
		ITEM_DISPLAY_TITLE_FONT_FACE, ITEM_DISPLAY_TITLE_FONT_SIZE );

	infoFont_ = FontFactory::create_font(
		ITEM_DISPLAY_INFO_FONT_FACE, ITEM_DISPLAY_INFO_FONT_SIZE );
}

void ItemDisplay::Release( void )
{
	if (nameFont_ != nullptr) {
		delete nameFont_;
		nameFont_ = nullptr;
	}

	if (infoFont_ != nullptr ) {
		delete infoFont_;
		infoFont_ = nullptr;
	}
}