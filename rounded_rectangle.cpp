#include "rounded_rectangle.h"

#define COLOUR_WHITE D3DCOLOR_XRGB( 255, 255, 255 )

RoundedRectangle::RoundedRectangle( int width, int height, int radius, D3DCOLOR colour )
{
	UnsetTexture();
	SetStroke( 0, D3DCOLOR_XRGB( 0, 0, 0 ) );
	SetStrokeType( STROKE_TYPE_OUTER );
	SetCornerRadius( radius );
	SetColour( colour );
	SetSize( width, height );
}

RoundedRectangle::~RoundedRectangle( void )
{
	RemoveTexture();
}

void RoundedRectangle::Draw( DirectX *directX )
{
	// Check if we need to redraw.
	if (roundedRect_ == nullptr) {
		Generate( directX );
	}

	// Draw texture.
	float textureX = GetX();
	float textureY = GetY();
	if (GetStrokeType() == STROKE_TYPE_OUTER) {
		int strokeSize = GetStrokeSize();
		textureX -= strokeSize;
		textureY -= strokeSize;
	}

	directX->DrawTexture(
		roundedRect_,
		textureX, textureY,
		roundedRect_->GetWidth(), roundedRect_->GetHeight(),
		D3DCOLOR_RGBA( 255, 255, 255, GetAlpha() ) );
}

void RoundedRectangle::SetStroke( int size, D3DCOLOR colour )
{
	strokeSize_ = size;
	strokeColour_ = colour;
}

void RoundedRectangle::SetColour( D3DCOLOR colour )
{
	colour_ = colour;
}

void RoundedRectangle::SetSize( int width, int height )
{
	Component::SetSize( width, height );
	RemoveTexture();
}

void RoundedRectangle::SetCornerRadius( int radius )
{
	radius_ = radius;
}

void RoundedRectangle::Generate( DirectX *directX )
{
	int rectWidth = GetWidth();
	int rectHeight = GetHeight();
	if (GetStrokeType() == STROKE_TYPE_OUTER) {
		rectWidth += GetStrokeSize() << 1;
		rectHeight += GetStrokeSize() << 1;
	}
	roundedRect_ = directX->CreateTexture( rectWidth, rectHeight );
	directX->SetRenderTarget( roundedRect_ );
	directX->SetProjectionSize( rectWidth, rectHeight );

	// Adjust radius of inner and outer based on stroke type.
	int innerRadius = radius_;
	int outerRadius = radius_;
	int strokeSize = GetStrokeSize();
	EStrokeType strokeType = GetStrokeType();
	if (strokeType == STROKE_TYPE_OUTER) {
		outerRadius += strokeSize;
	}
	else {
		innerRadius -= strokeSize;
	}

	// Draw outer rectangle if needed.
	if (strokeSize != 0) {
		directX->DrawRoundedRect(
			0, 0,
			rectWidth, rectHeight,
			outerRadius,
			GetStrokeColour() );
	}

	// Draw inner rectangle.
	directX->DrawRoundedRect(
		strokeSize, strokeSize,
		rectWidth - strokeSize * 2, rectHeight - strokeSize * 2,
		innerRadius,
		GetColour() );

	directX->ResetProjectionSize();
	directX->ResetRenderTarget();
}

Texture *RoundedRectangle::GetTexture( void ) const
{
	return roundedRect_;
}

void RoundedRectangle::UnsetTexture( void )
{
	roundedRect_ = nullptr;
}

void RoundedRectangle::RemoveTexture( void )
{
	if (roundedRect_ != nullptr) {
		delete roundedRect_;
		UnsetTexture();
	}
}

D3DCOLOR RoundedRectangle::GetColour( void ) const
{
	return colour_;
}

D3DCOLOR RoundedRectangle::GetStrokeColour( void ) const
{
	return strokeColour_;
}

int RoundedRectangle::GetStrokeSize( void ) const
{
	return strokeSize_;
}

void RoundedRectangle::SetStrokeType( EStrokeType strokeType )
{
	strokeType_ = strokeType;
}

EStrokeType RoundedRectangle::GetStrokeType( void ) const
{
	return strokeType_;
}
