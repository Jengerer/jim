#include "Option.h"

Option::Option( const string& caption, Texture *texture ) : Button( caption, texture ) 
{
	pack();
}

Option::~Option()
{
	// Option destroyed.
}

void Option::draw( DirectX *directX )
{
	// Set colour.
	D3DCOLOR buttonColour	= (isHovering_ ? BUTTON_COLOUR_HOVER : BUTTON_COLOUR );
	D3DCOLOR fontColour		= (isHovering_ ? BUTTON_FONT_HOVER : BUTTON_FONT_NORMAL );

	// Draw button base.
	float x = getX(), y = getY();
	directX->drawRoundedRect( getX(), getY(), getWidth(), getHeight(), MENU_BUTTON_RADIUS, buttonColour );

	// Draw text in center.
	RECT rect;
	rect.left = (long)x + MENU_BUTTON_PADDING_X;
	rect.top = (long)y + MENU_BUTTON_PADDING_Y;
	rect.right = (long)x + getWidth() - MENU_BUTTON_PADDING_X;
	rect.bottom = (long)y + getHeight() - MENU_BUTTON_PADDING_Y;

	// Write it.
	font->drawText(
		caption_,
		&rect,
		DT_SINGLELINE | DT_VCENTER, 
		fontColour );
}

void Option::pack()
{
	// Get size.
	RECT rect;
	font->getTextRect( caption_, &rect, DT_SINGLELINE );
	int width = (rect.right - rect.left) + MENU_BUTTON_PADDING_X * 2;
	int height = (rect.bottom - rect.top) + MENU_BUTTON_PADDING_Y * 2;

	// Set size.
	setSize( width, height );
}