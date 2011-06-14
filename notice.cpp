#include "notice.h"

// Static members.
Font*		Notice::font_	= nullptr;

#define NOTICE_FONT_FACE		"TF2 Build"
#define NOTICE_FONT_SIZE		20
#define NOTICE_FONT_BOLDED	false

Notice::Notice( const string& message ) : Popup( 0.0f, 0.0f )
{
	SetMessage( message );
}

Notice::~Notice()
{
	// Notice is destroyed.
}

// TODO: All to components.
void Notice::OnDraw( DirectX* directX )
{
	// Draw the stroke.
	directX->DrawRoundedRect( GetGlobalX(), GetGlobalY(), 
		GetWidth(), GetHeight(), 
		NOTICE_RADIUS, 
		NOTICE_STROKE_COLOUR ) ;

	// Draw inner rectangle.
	directX->DrawRoundedRect(
		GetGlobalX() + NOTICE_STROKE_WIDTH, 
		GetGlobalY() + NOTICE_STROKE_WIDTH, 
		GetWidth() - NOTICE_STROKE_WIDTH*2, 
		GetHeight() - NOTICE_STROKE_WIDTH*2, 
		NOTICE_RADIUS - NOTICE_STROKE_WIDTH, 
		NOTICE_COLOUR );

	long x = (long)GetGlobalX();
	long y = (long)GetGlobalY();

	// Center the text.
	RECT rect;
	rect.left	= x + NOTICE_PADDING + NOTICE_STROKE_WIDTH;
	rect.top	= y + NOTICE_PADDING + NOTICE_STROKE_WIDTH;
	rect.right	= x + NOTICE_WIDTH	- NOTICE_PADDING - NOTICE_STROKE_WIDTH;
	rect.bottom	= y + GetHeight()	- NOTICE_PADDING - NOTICE_STROKE_WIDTH;

	// Draw it.
	font_->drawText(message_, &rect, DT_CENTER, D3DCOLOR_ARGB( 255, 255, 255, 255 ));
}

void Notice::Pack( void )
{
	// Calculate size of text.
	RECT rect = {NOTICE_PADDING + NOTICE_STROKE_WIDTH, 0, NOTICE_WIDTH - NOTICE_PADDING - NOTICE_STROKE_WIDTH, 0};
	font_->wrapText( message_, &rect, DT_CENTER );

	// Store old, and set new.
	int oldHeight = GetHeight();
	int newHeight = (rect.bottom - rect.top) + NOTICE_PADDING*2 + NOTICE_STROKE_WIDTH*2;
	SetSize( NOTICE_WIDTH, newHeight );
}

void Notice::SetMessage( const string& message )
{
	message_ = message;
	Pack();
}

void Notice::AppendMessage( const string& message )
{
	SetMessage( message_ + message );
}

void Notice::Precache( DirectX *directX )
{
	font_ = directX->CreateFont( NOTICE_FONT_FACE, NOTICE_FONT_SIZE, NOTICE_FONT_BOLDED );
}

void Notice::Release( void )
{
	if (font_ != nullptr) {
		delete font_;
		font_ = nullptr;
	}
}