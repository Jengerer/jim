#include "Dialog.h"

Texture*	Dialog::texture	= 0;
const int	DIALOG_PADDING	= 20;
const int	DIALOG_WIDTH	= 250;
const D3DCOLOR DIALOG_COLOUR	= D3DCOLOR_XRGB( 10, 9, 7 );

Dialog::Dialog( const string& message )
{
	setMessage( message );
}

Dialog::~Dialog()
{
	// Dialog is destroyed.
}

void Dialog::draw( DirectX* directX )
{
	// Draw the background texture.
	directX->drawRoundedRect( getX(), getY(), getWidth(), getHeight(), 10, DIALOG_COLOUR );

	// Center the text.
	RECT rect;
	rect.left	= (long)getX() + DIALOG_PADDING;
	rect.top	= (long)getY() + DIALOG_PADDING;
	rect.right	= (long)getX() + getWidth()		- DIALOG_PADDING;
	rect.bottom	= (long)getY() + getHeight()	- DIALOG_PADDING;

	// Draw it.
	directX->drawText(message_, &rect, DT_CENTER | DT_WORDBREAK, D3DCOLOR_ARGB( 255, 255, 255, 255 ));
}

void Dialog::resize()
{
	// Get screen device context.
	HDC hdc = GetDC( NULL );

	// Calculate size of text.
	RECT size = {0, 0, DIALOG_WIDTH, 0};
	DrawText( hdc, message_.c_str(), -1, &size, DT_CALCRECT | DT_WORDBREAK );

	// Store old, and set new.
	int oldWidth = getWidth();
	int oldHeight = getHeight();
	setSize( size.right - size.left + DIALOG_PADDING*2, size.bottom - size.top + DIALOG_PADDING*2);

	// Keep alignment.
	float diffWidth = getWidth() - oldWidth;
	float diffHeight = getHeight() - oldHeight;
	float newX = getX() - diffWidth/2;
	float newY = getY() - diffHeight/2;
	setPosition( (int)newX, (int)newY );
}

void Dialog::setMessage( const string& message )
{
	message_ = message;
	resize();
}

void Dialog::appendMessage( const string& message )
{
	setMessage( message_ + message );
}