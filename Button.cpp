#include "Button.h"

Texture* Button::texture = 0;

Button::Button( const string& caption, float x, float y ): Drawable( x, y )
{
	isActive_ = false;
	caption_ = caption;
}

void Button::draw( DirectX* directX )
{
	// Colour if mouse rolling over.
	D3DCOLOR colour = ( isActive_ ? 
		D3DCOLOR_ARGB( 255, 180, 80, 15 ) : D3DCOLOR_ARGB( 255, 255, 255, 255 ) );

	// Draw button base.
	directX->drawTexture( texture, x, y, colour );
	
	// Draw text in center.
	RECT rect;
	rect.left = (long)x;
	rect.top = (long)y;
	rect.right = rect.left + getWidth();
	rect.bottom = rect.top + getHeight();

	// Write it.
	directX->drawText( 
		caption_, 
		&rect, 
		DT_CENTER | DT_SINGLELINE | DT_VCENTER, 
		D3DCOLOR_XRGB( 0, 0, 0 ) );
}

int Button::getWidth() const
{
	return texture->getWidth();
}

int Button::getHeight() const
{
	return texture->getHeight();
}