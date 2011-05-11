#include "Notification.h"

// Static members.
Texture*	Notification::texture_	= nullptr;
Font*		Notification::font_		= nullptr;

#define NOTIFICATION_FONT_FACE		"TF2 Build"
#define NOTIFICATION_FONT_SIZE		20
#define NOTIFICATION_FONT_BOLDED	false

Notification::Notification( const string& message ): Popup( 0.0f, 0.0f )
{
	SetMessage( message );
}

Notification::~Notification()
{
	// Notification is destroyed.
}

void Notification::OnDraw( DirectX* directX )
{
	// Draw the stroke.
	directX->drawRoundedRect( GetX(), GetY(), 
		GetWidth(), GetHeight(), 
		NOTIFICATION_RADIUS, 
		NOTIFICATION_STROKE_COLOUR ) ;

	// Draw inner rectangle.
	directX->drawRoundedRect(
		GetX() + NOTIFICATION_STROKE_WIDTH, 
		GetY() + NOTIFICATION_STROKE_WIDTH, 
		GetWidth() - NOTIFICATION_STROKE_WIDTH*2, 
		GetHeight() - NOTIFICATION_STROKE_WIDTH*2, 
		NOTIFICATION_RADIUS - NOTIFICATION_STROKE_WIDTH, 
		NOTIFICATION_COLOUR );

	long x = (long)GetX();
	long y = (long)GetY();

	// Center the text.
	RECT rect;
	rect.left	= x + NOTIFICATION_PADDING + NOTIFICATION_STROKE_WIDTH;
	rect.top	= y + NOTIFICATION_PADDING + NOTIFICATION_STROKE_WIDTH;
	rect.right	= x + NOTIFICATION_WIDTH	- NOTIFICATION_PADDING - NOTIFICATION_STROKE_WIDTH;
	rect.bottom	= y + GetHeight()	- NOTIFICATION_PADDING - NOTIFICATION_STROKE_WIDTH;

	// Draw it.
	font_->drawText(message_, &rect, DT_CENTER, D3DCOLOR_ARGB( 255, 255, 255, 255 ));
}

void Notification::Pack( void )
{
	// Calculate size of text.
	RECT rect = {NOTIFICATION_PADDING + NOTIFICATION_STROKE_WIDTH, 0, NOTIFICATION_WIDTH - NOTIFICATION_PADDING - NOTIFICATION_STROKE_WIDTH, 0};
	font_->wrapText( message_, &rect, DT_CENTER );

	// Store old, and set new.
	int oldHeight = GetHeight();
	int newHeight = (rect.bottom - rect.top) + NOTIFICATION_PADDING*2 + NOTIFICATION_STROKE_WIDTH*2;
	SetSize( NOTIFICATION_WIDTH, newHeight );

	// Keep alignment.
	float diffHeight = newHeight - oldHeight;
	int newY = GetY() - diffHeight/2;
	SetPosition( GetX(), newY );
}

void Notification::SetMessage( const string& message )
{
	message_ = message;
	Pack();
}

void Notification::AppendMessage( const string& message )
{
	SetMessage( message_ + message );
}

void Notification::Precache( DirectX *directX )
{
	font_ = directX->createFont( NOTIFICATION_FONT_FACE, NOTIFICATION_FONT_SIZE, NOTIFICATION_FONT_BOLDED );
}

void Notification::Release( void )
{
	if (font_ != nullptr) {
		delete font_;
		font_ = nullptr;
	}
}