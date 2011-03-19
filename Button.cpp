#include "Button.h"

Font*		Button::font = 0;

Button::Button( const string& caption, Texture* texture, float x, float y, EAlignment align )
{
	isHovering_ = false;
	caption_ = caption;

	// Set texture.
	setTexture( texture );
	pack();

	if (align == ALIGN_TOP_LEFT || align == ALIGN_BOTTOM_LEFT) {
		setX( x );
	}
	else {
		setX( x - getWidth() );
	}

	if (align == ALIGN_TOP_LEFT || align == ALIGN_TOP_RIGHT) {
		setY( y );
	}
	else {
		setY( y - getHeight() );
	}

	enable();
}

Button::~Button()
{
	// Button has been destroyed.
}

void Button::draw( DirectX* directX )
{
	// Set colour.
	D3DCOLOR buttonColour, fontColour;
	if (enabled_) {
		buttonColour = (isHovering_ ? BUTTON_COLOUR_HOVER : BUTTON_COLOUR );
		fontColour = (isHovering_ ? BUTTON_FONT_HOVER : BUTTON_FONT_NORMAL );
	}
	else {
		buttonColour = BUTTON_COLOUR_DISABLED;
		fontColour = BUTTON_FONT_DISABLED;
	}

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

bool Button::mouseMoved( Mouse *mouse )
{
	// Mouse moved.
	isHovering_ = mouse->isTouching( this );
	return isHovering_;
}

void Button::enable()
{
	enabled_ = true;
}

void Button::disable()
{
	enabled_ = false;
}

bool Button::isEnabled() const {
	return enabled_;
}

void Button::setTexture( Texture *texture )
{
	texture_ = texture;
}

void Button::pack()
{
	// Get caption size.
	RECT rect;
	font->getTextRect( caption_, &rect, DT_SINGLELINE );
	int width = (rect.right - rect.left) + BUTTON_PADDING_X * 2;
	int contentHeight = rect.bottom - rect.top;

	// Check if expansion for texture needed.
	if (texture_ != 0) {
		width += BUTTON_ICON_SIZE + BUTTON_ICON_SPACING;

		// Get maximum of content and texture height.
		if (BUTTON_ICON_SIZE > contentHeight) {
			contentHeight = BUTTON_ICON_SIZE;
		}
	}

	// Pad and size.
	int height = contentHeight + BUTTON_PADDING_Y * 2;
	setSize( width, height );
}