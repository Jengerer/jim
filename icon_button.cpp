#include "IconButton.h"

#define ICON_SIZE	20

IconButton::IconButton( Texture *texture, float x, float y )
{
	// Create icon, add to layout.
	icon_ = new Image( x, y, texture );
	layout_->Add( icon_ );

	// Take parent behaviour.
	SetIcon( texture );
}

IconButton::~IconButton( void )
{
	// IconButton destroyed.
}

void IconButton::SetIcon( Texture *texture )
{
	icon_->SetTexture( texture );
	if (texture != nullptr) {
		icon_->SetSize( ICON_SIZE, ICON_SIZE );
	}
	else {
		icon_->SetSize( 0, 0 );
	}
}

IconButton* IconButton::Create( Texture *texture, float x, float y )
{
	// Create button and pack.
	IconButton *button = new IconButton( texture, x, y );
	button->Pack();

	return button;
}

Image* IconButton::GetIcon( void ) const
{
	return icon_;
}