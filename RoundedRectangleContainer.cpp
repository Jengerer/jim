#include "RoundedRectangleContainer.h"

#define ROUNDED_RECTANGLE_DEFAULT_COLOUR	D3DCOLOR_XRGB( 255, 255, 255 )

RoundedRectangleContainer::RoundedRectangleContainer( unsigned int radius, float x, float y ) : Container( x, y )
{
	// Create rounded rectangle.
	roundedRect_ = new RoundedRectangle( 0, 0, radius, ROUNDED_RECTANGLE_DEFAULT_COLOUR );
	Add( roundedRect_ );

	// Set default parameters.
	SetRadius( radius );
	SetPadding( 0 );
}

RoundedRectangleContainer::~RoundedRectangleContainer( void )
{
	// Rounded rectangle destroyed.
}

void RoundedRectangleContainer::Pack( void )
{
	unsigned int padding = GetPadding();
	Component *contained = GetContained();
	contained->SetLocalPosition( padding, padding );
	roundedRect_->SetSize(
		contained->GetWidth() + (padding << 1),
		contained->GetHeight() + (padding << 1) );
	SetSize( roundedRect_->GetWidth(), roundedRect_->GetHeight() );
}

void RoundedRectangleContainer::SetContained( Component *component )
{
	contained_ = component;
}

void RoundedRectangleContainer::SetColour( D3DCOLOR colour )
{
	roundedRect_->SetColour( colour );
	roundedRect_->RemoveTexture();
}

void RoundedRectangleContainer::SetStroke( unsigned int size, D3DCOLOR colour )
{
	roundedRect_->SetStroke( size, colour );
}

void RoundedRectangleContainer::SetStrokeType( EStrokeType strokeType )
{
	roundedRect_->SetStrokeType( strokeType );
}

void RoundedRectangleContainer::SetRadius( unsigned int radius )
{
	roundedRect_->SetCornerRadius( radius );
}

void RoundedRectangleContainer::SetPadding( unsigned int padding )
{
	padding_ = padding;
}

Component* RoundedRectangleContainer::GetContained( void ) const
{
	return contained_;
}

unsigned int RoundedRectangleContainer::GetPadding( void ) const
{
	return padding_;
}