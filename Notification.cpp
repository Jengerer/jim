#include "Notification.h"

#define NOTIFICATION_ICON_SIZE		48
#define NOTIFICATION_TEXT_WIDTH		160
#define NOTIFICATION_COLOUR			D3DCOLOR_RGBA( 42, 39, 37, GetAlpha() )
#define NOTIFICATION_RADIUS			5
#define NOTIFICATION_PADDING		10
#define NOTIFICATION_SPACING		5

#define NOTIFICATION_STROKE_WIDTH	2
#define NOTIFICATION_STROKE_COLOUR	D3DCOLOR_RGBA( 255, 255, 255, GetAlpha() >> 1 )

#define NOTIFICATION_FONT_FACE		"TF2 Build"
#define NOTIFICATION_FONT_SIZE		14
#define NOTIFICATION_FONT_BOLDED	false
#define NOTIFICATION_FONT_COLOUR	D3DCOLOR_XRGB( 255, 255, 255 )

Font *Notification::font_ = nullptr;

Notification::Notification( const string& message, Texture *texture )
{
	// Create icon and text objects.
	text_ = new WrappedText( font_, NOTIFICATION_TEXT_WIDTH );
	text_->SetColour( NOTIFICATION_FONT_COLOUR );
	image_ = new Image( 0, 0, texture );

	// Add them to the layout.
	layout_ = new HorizontalLayout();
	layout_->SetSpacing( NOTIFICATION_SPACING );
	layout_->Add( image_ );
	layout_->Add( text_ );
	Add( layout_ );

	// Now set message, texture, and pack.
	SetMessage( message );
	SetTexture( texture );
	Pack();
}

Notification::~Notification( void )
{
	// Notification destroyed.
}

void Notification::OnDraw( DirectX *directX )
{
	// Draw rounded rectangle.
	directX->DrawRoundedRect(
		GetX() - NOTIFICATION_STROKE_WIDTH, GetY() - NOTIFICATION_STROKE_WIDTH,
		GetWidth() + NOTIFICATION_STROKE_WIDTH*2, GetHeight() + NOTIFICATION_STROKE_WIDTH * 2,
		NOTIFICATION_RADIUS + NOTIFICATION_STROKE_WIDTH, NOTIFICATION_RADIUS + NOTIFICATION_STROKE_WIDTH, NOTIFICATION_RADIUS + NOTIFICATION_STROKE_WIDTH, NOTIFICATION_RADIUS + NOTIFICATION_STROKE_WIDTH,
		NOTIFICATION_STROKE_COLOUR );

	// Draw rounded rectangle.
	directX->DrawRoundedRect(
		GetX(), GetY(),
		GetWidth(), GetHeight(),
		NOTIFICATION_RADIUS, NOTIFICATION_RADIUS, NOTIFICATION_RADIUS, NOTIFICATION_RADIUS,
		NOTIFICATION_COLOUR );

	// Draw layout.
	layout_->OnDraw( directX );
}

void Notification::Pack( void )
{
	// Pack layout, add padding.
	layout_->Pack();
	SetSize( 
		layout_->GetWidth() + NOTIFICATION_PADDING * 2,
		layout_->GetHeight() + NOTIFICATION_PADDING * 2 );
}

void Notification::UpdatePosition( void )
{
	// Just move the layout padding in.
	layout_->SetPosition(
		GetX() + NOTIFICATION_PADDING, 
		GetY() + NOTIFICATION_PADDING );
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