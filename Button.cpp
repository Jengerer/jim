#include "Button.h"

Font*		Button::font_ = nullptr;

#define BUTTON_FONT_FACE	"TF2 Build"
#define BUTTON_FONT_SIZE	20
#define BUTTON_FONT_BOLDED	false

Button::Button( const string& caption, float x, float y )
{
	SetHovering( false );
	SetCaption( caption );
	SetPosition( x, y );
	SetEnabled( true );
	SetIcon( nullptr );
	Pack();
}

Button::~Button()
{
	// Button has been destroyed.
}

void Button::OnDraw( DirectX* directX )
{
	// Set colour.
	D3DCOLOR buttonColour, fontColour;
	if ( IsEnabled() ) {
		buttonColour = (isHovering_ ? BUTTON_COLOUR_HOVER : BUTTON_COLOUR );
		fontColour = (isHovering_ ? BUTTON_FONT_HOVER : BUTTON_FONT_NORMAL );
	}
	else {
		buttonColour = BUTTON_COLOUR_DISABLED;
		fontColour = BUTTON_FONT_DISABLED;
	}

	// Draw button base.
	float x = GetX(), y = GetY();
	directX->DrawRoundedRect( GetX(), GetY(), GetWidth(), GetHeight(), BUTTON_RADIUS, 0, BUTTON_RADIUS, 0, buttonColour );

	if ( GetIcon() != nullptr ) {
		directX->DrawTexture( GetIcon(), GetX() + BUTTON_PADDING_X, GetY() + BUTTON_PADDING_Y, BUTTON_ICON_SIZE, BUTTON_ICON_SIZE );
	}

	// Draw text in center.
	RECT rect;
	rect.left = (long)x + ( GetIcon() != 0 ? BUTTON_ICON_SIZE + BUTTON_ICON_SPACING : 0 ) + BUTTON_PADDING_X;
	rect.top = (long)y + BUTTON_PADDING_Y;
	rect.right = (long)x + GetWidth() - BUTTON_PADDING_X;
	rect.bottom = (long)y + GetHeight() - BUTTON_PADDING_Y;

	// Write it.
	font_->drawText(
		caption_,
		&rect,
		DT_CENTER | DT_SINGLELINE | DT_VCENTER, 
		fontColour );
}

void Button::SetIcon( Texture *texture )
{
	icon_ = texture;
	Pack();
}

Texture* Button::GetIcon( void )
{
	return icon_;
}

void Button::SetCaption( const string& caption )
{
	caption_ = caption;
}

const string& Button::GetCaption( void ) const
{
	return caption_;
}

//=============================================================
// Purpose: Resizes the button to fit the button and icon.
//=============================================================
void Button::Pack( void )
{
	// Get caption size.
	RECT rect;
	font_->getTextRect( caption_, &rect, DT_SINGLELINE );
	int width = (rect.right - rect.left) + BUTTON_PADDING_X * 2;
	int contentHeight = rect.bottom - rect.top;

	// Check if expansion for icon needed.
	if (icon_ != nullptr) {
		width += BUTTON_ICON_SIZE + BUTTON_ICON_SPACING;
		if (BUTTON_ICON_SIZE > contentHeight) {
			contentHeight = BUTTON_ICON_SIZE;
		}
	}

	// Pad and size.
	int height = contentHeight + BUTTON_PADDING_Y * 2;
	SetSize( width, height );
}

void Button::SetEnabled( bool isEnabled )
{
	isEnabled_ = isEnabled;
}

bool Button::IsEnabled() const
{
	return isEnabled_;
}

bool Button::OnMouseMoved( Mouse *mouse )
{
	// Mouse moved.
	isHovering_ = mouse->IsTouching( this );
	return isHovering_;
}

bool Button::OnLeftClicked( Mouse *mouse )
{
	return mouse->IsTouching( this );
}

bool Button::OnLeftReleased( Mouse *mouse )
{
	return mouse->IsTouching( this );
}

bool Button::OnRightClicked( Mouse *mouse )
{
	return mouse->IsTouching( this );
}

bool Button::OnRightReleased( Mouse *mouse )
{
	return mouse->IsTouching( this );
}

void Button::Precache( DirectX *directX )
{
	font_ = directX->CreateFont( 
		BUTTON_FONT_FACE, 
		BUTTON_FONT_SIZE, 
		BUTTON_FONT_BOLDED );
}

void Button::Release( void )
{
	if (font_ != nullptr) {
		delete font_;
		font_ = nullptr;
	}
}

void Button::SetHovering( bool isHovering )
{
	isHovering_ = isHovering;
}

bool Button::IsHovering( void ) const
{
	return isHovering_;
}