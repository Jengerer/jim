#include "button.hpp"

#include <jui/gfx/font_factory.hpp>
#include <jui/gfx/image.hpp>
#include <jui/gfx/text.hpp>

JUI::FontInterface* Button::defaultFont_ = nullptr;

// Button icon size.
const unsigned int BUTTON_ICON_SIZE			= 20;

// Button default font size.
const char* BUTTON_DEFAULT_FONT_FACE = "fonts/tf2build.ttf";
const unsigned int BUTTON_DEFAULT_FONT_SIZE	= 14;
const JUI::Colour BUTTON_FONT_COLOUR( 42, 39, 37 );

// Rounded container parameters.
const unsigned int BUTTON_RADIUS		= 5;
const unsigned int BUTTON_SPACING		= 5;
const unsigned int BUTTON_PADDING		= 15;

// Button colours.
const JUI::Colour BUTTON_COLOUR( 247, 231, 198 );
const JUI::Colour BUTTON_COLOUR_HOVER( 180, 81, 14 );
const JUI::Colour BUTTON_COLOUR_DISABLED( 247, 231, 198, 150 );

Button::Button( float localX, float localY ) : RoundedRectangleContainer( BUTTON_RADIUS, BUTTON_PADDING, localX, localY )
{
	// Generate layout.
	layout_ = new JUI::HorizontalLayout( BUTTON_SPACING, ALIGN_MIDDLE );
	add( layout_ );

	// Set rounded container attributes.
	set_content( layout_ );

	// Default attributes.
	SetHovering( false );
	SetEnabled( true );
	UpdateButton();
}

void Button::pack( void )
{
	layout_->pack();
	RoundedRectangleContainer::pack();
}

Layout* Button::GetContentLayout() const
{
	return layout_;
}

void Button::UpdateButton( void )
{
	RoundedRectangle *roundedRect = get_rounded_rectangle();
	const JUI::Colour* oldColour = &roundedRect->get_colour();
	const JUI::Colour* newColour = nullptr;
	if ( IsEnabled() ) {
		if ( IsHovering() ) {
			newColour = &BUTTON_COLOUR_HOVER;
		}
		else {
			newColour = &BUTTON_COLOUR;
		}
	}
	else {
		newColour = &BUTTON_COLOUR_DISABLED;
	}

	roundedRect->set_colour( *newColour );
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

bool Button::on_mouse_moved( JUI::Mouse* mouse )
{
	// JUI::Mouse* moved.
	bool isHovering = mouse->is_touching( this );
	SetHovering( isHovering );
	return isHovering;
}

bool Button::on_mouse_clicked( JUI::Mouse* mouse )
{
	return IsEnabled() && mouse->is_touching( this );
}

bool Button::on_mouse_released( JUI::Mouse* mouse )
{
	return IsEnabled() && mouse->is_touching( this );
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

void Button::precache( JUI::Graphics2D* graphics )
{
	defaultFont_ = FontFactory::create_font( BUTTON_DEFAULT_FONT_FACE,
		BUTTON_DEFAULT_FONT_SIZE );
}

void Button::release()
{
	if (defaultFont_ != nullptr) {
		delete defaultFont_;
		defaultFont_ = nullptr;
	}
}

Button* Button::CreateIconButton( JUI::Texture* texture )
{
	Button* button = new Button;
	JUI::Layout* layout = button->GetContentLayout();

	JUI::Image *icon = new JUI::Image( texture );

	layout->add( icon );

	button->pack();
	return button;
}

Button* Button::create_label_button( const std::string& label, JUI::FontInterface* font )
{
	Button* button = new Button;
	JUI::Layout* layout = button->GetContentLayout();

	JUI::Text *text = new JUI::Text( font );
	text->set_colour( BUTTON_FONT_COLOUR );
	text->set_text( label );
	layout->add( text );
	button->pack();

	return button;
}

Button* Button::CreateIconLabelButton( JUI::Texture* texture, const std::string& label, JUI::FontInterface* font )
{
	Button* button = new Button;
	JUI::Layout* layout = button->GetContentLayout();

	JUI::Image* icon = new JUI::Image( texture );
	icon->set_size( BUTTON_ICON_SIZE, BUTTON_ICON_SIZE );

	JUI::Text* text = new JUI::Text( font );
	text->set_colour( BUTTON_FONT_COLOUR );
	text->set_text( label );
	layout->add( icon );
	layout->add( text );
	button->pack();

	return button;
}