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
	SetAlpha( 0 );
	GetRoundedRectangle()->SetColour( ITEM_DISPLAY_COLOUR );
	SetItem( nullptr );
	SetActive( false );

	// Create text objects.
	nameText_ = new WrappedText( nameFont_, ITEM_DISPLAY_TEXT_WIDTH );
	nameText_->SetTextFormatting( DT_CENTER );
	infoText_ = new WrappedText( infoFont_, ITEM_DISPLAY_TEXT_WIDTH );
	infoText_->SetTextFormatting( DT_CENTER );

	// Add to layout.
	textLayout_ = new VerticalLayout();
	textLayout_->SetSpacing( ITEM_DISPLAY_SPACING );
	textLayout_->Add( nameText_ );
	textLayout_->Add( infoText_ );

	// Pack so we can create a temporary rectangle.
	Add( textLayout_ );
	SetContent( textLayout_ );
	Pack();
}

ItemDisplay::~ItemDisplay( void )
{
}

void ItemDisplay::UpdateDisplay()
{
	// Alter display based on quality.
	nameText_->SetColour( item_->GetQualityColour() );
	SetName( item_->GetName() );

	// Build information text.
	stringstream infoStream;
	infoStream << "LEVEL " << static_cast<uint16>(item_->GetLevel());

#if defined( _DEBUG )
	infoStream << "\nFLAGS: " << hex << item_->GetFlags();
#endif

	for (size_t i = 0; i < item_->GetAttributeCount(); i++) {
		const Attribute* attrib = item_->GetAttribute( i );
		if (attrib->has_description()) {
			infoStream << "\n" << attrib->get_description_string();
		}
	}

	infoText_->SetText( infoStream.str() );
	Pack();
}

void ItemDisplay::UpdateAlpha( void )
{
	if ( IsActive() ) {
		SetAlpha( GetAlpha() + ITEM_DISPLAY_ALPHA_SPEED );
		if (GetAlpha() > ITEM_DISPLAY_ALPHA_MAX) {
			SetAlpha( ITEM_DISPLAY_ALPHA_MAX );
		}
	}
	else {
		SetAlpha( GetAlpha() - ITEM_DISPLAY_ALPHA_SPEED );
	}
}

void ItemDisplay::Pack( void )
{
	nameText_->Pack();
	infoText_->Pack();
	textLayout_->Pack();
	RoundedRectangleContainer::Pack();
}

const Item* ItemDisplay::GetItem( void ) const
{
	return item_;
}

void ItemDisplay::SetItem( const Item *item )
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

const string& ItemDisplay::GetName( void ) const
{
	return itemName_;
}

void ItemDisplay::SetName( const string& name )
{
	itemName_ = name;
	nameText_->SetText( name );
}

bool ItemDisplay::IsActive( void ) const
{
	return isActive_;
}

void ItemDisplay::SetActive( bool isActive )
{
	isActive_ = isActive;
}

void ItemDisplay::Precache()
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