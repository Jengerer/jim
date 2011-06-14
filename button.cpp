#include "Button.h"

Font*		Button::font_ = nullptr;

// Rounded rectangle parameters.
#define BUTTON_RADIUS		5
#define BUTTON_SPACING		5
#define BUTTON_PADDING		15

// Button colours.
#define BUTTON_COLOUR			D3DCOLOR_XRGB( 247, 231, 198 )
#define BUTTON_COLOUR_HOVER		D3DCOLOR_XRGB( 180, 81, 14 )
#define BUTTON_COLOUR_DISABLED	D3DCOLOR_ARGB( 150, 247, 231, 198)

Button::Button( float x, float y ) : RoundedRectangleContainer( BUTTON_RADIUS, x, y )
{
	// Generate layout.
	layout_ = new HorizontalLayout();
	layout_->SetSpacing( BUTTON_SPACING );
	layout_->SetAlignType( ALIGN_TOP );
	Add( layout_ );
	SetContained( layout_ );
	SetPadding( BUTTON_PADDING );

	// Default attributes.
	SetHovering( false );
	SetEnabled( true );
	UpdateButton();
}

Button::~Button()
{
	// Button has been destroyed.
}

void Button::Pack( void )
{
	layout_->Pack();
	RoundedRectangleContainer::Pack();
}

void Button::UpdateButton( void )
{
	D3DCOLOR colour;
	if ( IsEnabled() ) {
		if ( IsHovering() ) {
			colour = BUTTON_COLOUR_HOVER;
		}
		else {
			colour = BUTTON_COLOUR;
		}
	}
	else {
		colour = BUTTON_COLOUR_DISABLED;
	}

	SetColour( colour );
}

void Button::SetEnabled( bool isEnabled )
{
	isEnabled_ = isEnabled;
	UpdateButton();
}

bool Button::IsEnabled() const
{
	return isEnabled_;
}

bool Button::OnMouseMoved( Mouse *mouse )
{
	// Mouse moved.
	bool isHovering = mouse->IsTouching( this );
	SetHovering( isHovering );
	return isHovering;
}

bool Button::OnLeftClicked( Mouse *mouse )
{
	return IsEnabled() && mouse->IsTouching( this );
}

bool Button::OnLeftReleased( Mouse *mouse )
{
	return IsEnabled() && mouse->IsTouching( this );
}

void Button::SetHovering( bool isHovering )
{
	isHovering_ = isHovering;
	UpdateButton();
}

bool Button::IsHovering( void ) const
{
	return isHovering_;
}