#include "button.h"

#include "image.h"
#include "text.h"

Font* Button::defaultFont_ = nullptr;

// Button icon size.
const unsigned int BUTTON_ICON_SIZE			= 20;

// Button default font size.
const char* BUTTON_DEFAULT_FONT_FACE		= "TF2 Build";
const unsigned int BUTTON_DEFAULT_FONT_SIZE	= 20;
const bool BUTTON_DEFAULT_FONT_BOLDED		= false;
const D3DCOLOR BUTTON_FONT_COLOUR			= D3DCOLOR_XRGB( 42, 39, 37 );

// Rounded container parameters.
const unsigned int BUTTON_RADIUS		= 5;
const unsigned int BUTTON_SPACING		= 5;
const unsigned int BUTTON_PADDING		= 15;

// Button colours.
const D3DCOLOR BUTTON_COLOUR			= D3DCOLOR_XRGB( 247, 231, 198 );
const D3DCOLOR BUTTON_COLOUR_HOVER		= D3DCOLOR_XRGB( 180, 81, 14 );
const D3DCOLOR BUTTON_COLOUR_DISABLED	= D3DCOLOR_ARGB( 150, 247, 231, 198);

Button::Button( float localX, float localY ) : RoundedRectangleContainer( BUTTON_RADIUS, BUTTON_PADDING, localX, localY )
{
	// Generate layout.
	layout_ = new HorizontalLayout( BUTTON_SPACING, ALIGN_MIDDLE );
	Add( layout_ );

	// Set rounded container attributes.
	SetContent( layout_ );

	// Default attributes.
	SetHovering( false );
	SetEnabled( true );
	UpdateButton();
}

void Button::Pack( void )
{
	layout_->Pack();
	RoundedRectangleContainer::Pack();
}

Layout* Button::GetContentLayout() const
{
	return layout_;
}

void Button::UpdateButton( void )
{
	RoundedRectangle *roundedRect = GetRoundedRectangle();
	D3DCOLOR oldColour = roundedRect->GetColour();
	D3DCOLOR newColour;
	if ( IsEnabled() ) {
		if ( IsHovering() ) {
			newColour = BUTTON_COLOUR_HOVER;
		}
		else {
			newColour = BUTTON_COLOUR;
		}
	}
	else {
		newColour = BUTTON_COLOUR_DISABLED;
	}

	if (newColour != oldColour) {
		roundedRect->SetColour( newColour );
		roundedRect->RemoveTexture();
	}
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

void Button::Precache( DirectX *directX )
{
	defaultFont_ = directX->CreateFont( BUTTON_DEFAULT_FONT_FACE,
		BUTTON_DEFAULT_FONT_SIZE,
		BUTTON_DEFAULT_FONT_BOLDED );
}

void Button::Release()
{
	if (defaultFont_ != nullptr) {
		delete defaultFont_;
		defaultFont_ = nullptr;
	}
}

Button* Button::CreateIconButton( Texture* texture )
{
	Button* button = new Button;
	Layout* layout = button->GetContentLayout();

	Image *icon = new Image( texture );

	layout->Add( icon );

	button->Pack();
	return button;
}

Button* Button::CreateLabelButton( const string& label, Font* font )
{
	Button* button = new Button;
	Layout* layout = button->GetContentLayout();

	Text *text = new Text( font );
	text->SetColour( BUTTON_FONT_COLOUR );
	text->SetText( label );
	text->Pack();

	layout->Add( text );
	button->Pack();

	return button;
}

Button* Button::CreateIconLabelButton( Texture* texture, const string& label, Font* font )
{
	Button* button = new Button;
	Layout* layout = button->GetContentLayout();

	Image* icon = new Image( texture );
	icon->SetSize( BUTTON_ICON_SIZE, BUTTON_ICON_SIZE );

	Text* text = new Text( font );
	text->SetColour( BUTTON_FONT_COLOUR );
	text->SetText( label );
	text->Pack();

	layout->Add( icon );
	layout->Add( text );
	button->Pack();

	return button;
}