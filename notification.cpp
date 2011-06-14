#include "notification.h"

#define NOTIFICATION_ICON_SIZE		48
#define NOTIFICATION_TEXT_WIDTH		160
#define NOTIFICATION_COLOUR			D3DCOLOR_XRGB( 42, 39, 37 )
#define NOTIFICATION_RADIUS			5
#define NOTIFICATION_PADDING		10
#define NOTIFICATION_SPACING		5

#define NOTIFICATION_STROKE_SIZE	2
#define NOTIFICATION_STROKE_COLOUR	D3DCOLOR_XRGB( 255, 255, 255 )

#define NOTIFICATION_FONT_FACE		"TF2 Build"
#define NOTIFICATION_FONT_SIZE		14
#define NOTIFICATION_FONT_BOLDED	false
#define NOTIFICATION_FONT_COLOUR	D3DCOLOR_XRGB( 255, 255, 255 )

Font *Notification::font_ = nullptr;

Notification::Notification( const string& message, Texture *texture ) : RoundedRectangleContainer( NOTIFICATION_RADIUS )
{
	// Create icon and text objects.
	text_ = new WrappedText( font_, NOTIFICATION_TEXT_WIDTH );
	text_->SetColour( NOTIFICATION_FONT_COLOUR );
	image_ = new Image( 0, 0, texture );

	// Add them to the layout.
	layout_ = new HorizontalLayout();
	layout_->SetSpacing( NOTIFICATION_SPACING );
	layout_->SetAlignType( ALIGN_TOP );
	layout_->Add( image_ );
	layout_->Add( text_ );
	Add( layout_ );
	SetContained( layout_ );
	SetPadding( NOTIFICATION_PADDING );
	SetColour( NOTIFICATION_COLOUR );
	SetStroke( NOTIFICATION_STROKE_SIZE, NOTIFICATION_STROKE_COLOUR );

	// Now set message, texture, and pack.
	SetMessage( message );
	SetTexture( texture );
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

void Notification::SetTexture( Texture *texture )
{
	image_->SetTexture( texture );
	if (texture != nullptr) {
		image_->SetSize( NOTIFICATION_ICON_SIZE, NOTIFICATION_ICON_SIZE );
	}
	else {
		image_->SetSize( 0, 0 );
	}
}

void Notification::SetMessage( const string& message )
{
	text_->SetText( message );
}

void Notification::Precache( DirectX *directX )
{
	font_ = directX->CreateFont( NOTIFICATION_FONT_FACE,
		NOTIFICATION_FONT_SIZE,
		NOTIFICATION_FONT_BOLDED );
}

void Notification::Release( void )
{
	if (font_ != nullptr) {
		delete font_;
		font_ = nullptr;
	}
}