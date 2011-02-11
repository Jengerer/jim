#include "Button.h"

Font*		Button::font = 0;

Button::Button( const string& caption )
{
	// Default behaviour.
	isHovering_ = false;
	caption_ = caption;
	texture_ = 0;
}

Button::Button( const string& caption, Texture* texture, float x, float y, EAlignment align )
{
	isHovering_ = false;
	caption_ = caption;

	// Set texture.
	texture_ = texture;

	// Get size.
	RECT rect;
	font->getTextRect( caption, &rect, DT_SINGLELINE );
	int width = (rect.right - rect.left) + BUTTON_PADDING_X * 2;
	int contentHeight = rect.bottom - rect.top;

	// Check if expansion for texture needed.
	if (texture != 0) {
		width += BUTTON_ICON_SIZE + BUTTON_ICON_SPACING;

		// Get maximum of content and texture height.
		if (BUTTON_ICON_SIZE > contentHeight) {
			contentHeight = BUTTON_ICON_SIZE;
		}
	}

	// Now add padding.
	int height = contentHeight + BUTTON_PADDING_Y * 2;

	// Set size.
	setSize( width, height );

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
	// Set colour.
	D3DCOLOR buttonColour	= (isHovering_ ? BUTTON_COLOUR_HOVER : BUTTON_COLOUR );
	D3DCOLOR fontColour		= (isHovering_ ? BUTTON_FONT_HOVER : BUTTON_FONT_NORMAL );

	// Draw button base.
	float x = getX(), y = getY();
	directX->drawRoundedRect( getX(), getY(), getWidth(), getHeight(), BUTTON_RADIUS, buttonColour );

	if (texture_) {
		directX->drawTexture( texture_, getX() + BUTTON_PADDING_X, getY() + BUTTON_PADDING_Y, BUTTON_ICON_SIZE, BUTTON_ICON_SIZE );
	}

	// Draw text in center.
	RECT rect;
	rect.left = (long)x + ( texture_ != 0 ? BUTTON_ICON_SIZE + BUTTON_ICON_SPACING : 0 ) + BUTTON_PADDING_X;
	rect.top = (long)y + BUTTON_PADDING_Y;
	rect.right = (long)x + getWidth() - BUTTON_PADDING_X;
	rect.bottom = (long)y + getHeight() - BUTTON_PADDING_Y;

	// Write it.
	font->drawText(
		caption_,
		&rect,
		DT_CENTER | DT_SINGLELINE | DT_VCENTER, 
		fontColour );
}

bool Button::mouseMoved( Mouse *mouse ) {
	// Mouse moved.
	isHovering_ = mouse->isTouching( this );
	return isHovering_;
}