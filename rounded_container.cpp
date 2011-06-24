#include "rounded_container.h"

const D3DCOLOR ROUNDED_RECTANGLE_DEFAULT_COLOUR = D3DCOLOR_XRGB( 255, 255, 255 );


RoundedRectangleContainer::RoundedRectangleContainer( unsigned int radius, unsigned int padding, float localX, float localY ) : Container( localX, localY )
{
	SetPadding( padding );

	// Create rounded rectangle.
	roundedRect_ = new RoundedRectangle( padding_, padding_, radius, ROUNDED_RECTANGLE_DEFAULT_COLOUR );
	Add( roundedRect_ );

	// Create content container.
	content_ = new HorizontalLayout();
	Add( content_ );
}

void RoundedRectangleContainer::Pack( void )
{
	// Inset content by padding.
	content_->SetLocalPosition( padding_, padding_ );

	// Set rectangle size.
	int rectWidth = content_->GetWidth() + (padding_ * 2);
	int rectHeight = content_->GetHeight() + (padding_ * 2);
	roundedRect_->SetSize( rectWidth, rectHeight );
	SetSize( rectWidth, rectHeight );
}

void RoundedRectangleContainer::SetContent( Component* content )
{
	content_ = content;
}

RoundedRectangle* RoundedRectangleContainer::GetRoundedRectangle() const
{
	return roundedRect_;
}

void RoundedRectangleContainer::SetPadding( unsigned int padding )
{
	padding_ = padding;
}