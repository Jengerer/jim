#include "notification.h"
#include "font_factory.h"

const unsigned int NOTIFICATION_ICON_SIZE	= 48;
const unsigned int NOTIFICATION_TEXT_WIDTH	= 160;
const Colour NOTIFICATION_COLOUR			= Colour( 42, 39, 37 );
const unsigned int NOTIFICATION_RADIUS		= 5;
const unsigned int NOTIFICATION_PADDING		= 10;
const unsigned int NOTIFICATION_SPACING		= 5;

const unsigned int NOTIFICATION_STROKE_SIZE	= 2;
const Colour& NOTIFICATION_STROKE_COLOUR	= COLOUR_WHITE;

const char* NOTIFICATION_FONT_FACE			= "fonts/tf2build.ttf";
const unsigned int NOTIFICATION_FONT_SIZE	= 14;
const Colour& NOTIFICATION_FONT_COLOUR		= COLOUR_WHITE;

Font *Notification::font_ = nullptr;

Notification::Notification( const string& message, Texture *texture ) : RoundedRectangleContainer( NOTIFICATION_RADIUS, NOTIFICATION_PADDING )
{
	text_ = new WrappedText( font_, NOTIFICATION_TEXT_WIDTH );
	text_->SetColour( NOTIFICATION_FONT_COLOUR );

	layout_ = new HorizontalLayout();
	layout_->SetSpacing( NOTIFICATION_SPACING );
	layout_->SetAlignType( ALIGN_MIDDLE );
	Add( layout_ );
	SetContent( layout_ );

	// Only add image if there is one.
	image_ = nullptr;
	if (texture != nullptr) {
		image_ = new Image( texture );
		image_->SetSize( NOTIFICATION_ICON_SIZE, NOTIFICATION_ICON_SIZE );
		layout_->Add( image_ );
	}

	layout_->Add( text_ );

	// Set rectangle attributes.
	RoundedRectangle* roundedRect = GetRoundedRectangle();
	roundedRect->SetColour( NOTIFICATION_COLOUR );
	roundedRect->SetStroke( NOTIFICATION_STROKE_SIZE, NOTIFICATION_STROKE_COLOUR );
	roundedRect->RemoveTexture();

	// Now set message, texture, and pack.
	SetMessage( message );
	Pack();
}

Notification::~Notification( void )
{
	// Notification destroyed.
}

void Notification::Pack( void )
{
	// Pack layout, add padding.
	layout_->Pack();
	RoundedRectangleContainer::Pack();
}

void Notification::SetMessage( const string& message )
{
	text_->SetText( message );
}

void Notification::Precache()
{
	font_ = FontFactory::create_font( NOTIFICATION_FONT_FACE, NOTIFICATION_FONT_SIZE );
}

void Notification::Release( void )
{
	if (font_ != nullptr) {
		delete font_;
		font_ = nullptr;
	}
}