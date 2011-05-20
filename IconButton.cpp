#include "IconButton.h"

#define ICON_SIZE	20

IconButton::IconButton( Texture *texture, float x, float y )
{
	// Create icon, add to layout.
	icon_ = new Image( x, y, texture );
	icon_->SetSize( ICON_SIZE, ICON_SIZE );
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
}

Image* IconButton::GetIcon( void ) const
{
	return icon_;
}