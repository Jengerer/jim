#include "Dialog.h"

// Static members.
Texture*	Dialog::texture			= 0;
Font*		Dialog::font			= 0;

// Colours.
const D3DCOLOR DIALOG_STROKE_COLOUR	= D3DCOLOR_RGBA( 153, 142, 121, 100 );
const D3DCOLOR DIALOG_COLOUR		= D3DCOLOR_XRGB( 42, 39, 37 );

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
	// Draw the stroke.
	directX->drawRoundedRect( 
		getX(), 
		getY(), 
		getWidth(), 
		getHeight(), 
		DIALOG_RADIUS, 
		DIALOG_STROKE_COLOUR ) ;

	// Draw inner rectangle.
	directX->drawRoundedRect(
		getX() + DIALOG_STROKE_WIDTH, 
		getY() + DIALOG_STROKE_WIDTH, 
		getWidth() - DIALOG_STROKE_WIDTH*2, 
		getHeight() - DIALOG_STROKE_WIDTH*2, 
		DIALOG_RADIUS - DIALOG_STROKE_WIDTH, 
		DIALOG_COLOUR );

	long x = (long)getX();
	long y = (long)getY();

	// Center the text.
	RECT rect;
	rect.left	= x + DIALOG_PADDING + DIALOG_STROKE_WIDTH;
	rect.top	= y + DIALOG_PADDING + DIALOG_STROKE_WIDTH;
	rect.right	= x + DIALOG_WIDTH	- DIALOG_PADDING - DIALOG_STROKE_WIDTH;
	rect.bottom	= y + getHeight()	- DIALOG_PADDING - DIALOG_STROKE_WIDTH;

	// Draw it.
	font->drawText(message_, &rect, DT_WORDBREAK | DT_CENTER, D3DCOLOR_ARGB( 255, 255, 255, 255 ));
}

void Dialog::resize()
{
	// Calculate size of text.
	RECT rect = {DIALOG_PADDING + DIALOG_STROKE_WIDTH, 0, DIALOG_WIDTH - DIALOG_PADDING - DIALOG_STROKE_WIDTH, 0};
	font->getTextRect( message_, &rect, DT_WORDBREAK | DT_CENTER );

	// Store old, and set new.
	int oldHeight = getHeight();
	int newHeight = (rect.bottom - rect.top) + DIALOG_PADDING*2 + DIALOG_STROKE_WIDTH*2;
	setSize( DIALOG_WIDTH, newHeight );

	// Keep alignment.
	float diffHeight = newHeight - oldHeight;
	int newY = getY() - diffHeight/2;
	setPosition( getX(), newY );
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