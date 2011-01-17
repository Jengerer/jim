#include "Button.h"

Font*		Button::font = 0;

Button::Button( const string& caption, float x, float y, EAlignment align )
{
	isActive_ = false;
	caption_ = caption;

	// Get size.
	RECT rect;
	font->getTextRect( caption, &rect, DT_SINGLELINE );
	setSize( rect.right - rect.left + BUTTON_PADDING_X * 2, rect.bottom - rect.top + BUTTON_PADDING_Y*2 );

	// Position based on alignment.
	setX( (align == ALIGN_TOP_LEFT || align == ALIGN_BOTTOM_LEFT) ? x : x - getWidth() );
	setY( (align == ALIGN_TOP_LEFT || align == ALIGN_TOP_RIGHT) ? y : y - getHeight() );
}

Button::~Button()
{
	// Button has been destroyed.
}

void Button::draw( DirectX* directX )
{
	// Draw button base.
	float x = getX(), y = getY();
	directX->drawRoundedRect( getX(), getY(), getWidth(), getHeight(), BUTTON_RADIUS, BUTTON_COLOUR );
	
	// Draw text in center.
	RECT rect;
	rect.left = (long)x + BUTTON_PADDING_X;
	rect.top = (long)y + BUTTON_PADDING_Y;
	rect.right = (long)x + getWidth() - BUTTON_PADDING_X;
	rect.bottom = (long)y + getHeight() - BUTTON_PADDING_Y;

	// Write it.
	font->drawText(
		caption_, 
		&rect, 
		DT_CENTER | DT_SINGLELINE | DT_VCENTER, 
		D3DCOLOR_XRGB( 0, 0, 0 ) );
}