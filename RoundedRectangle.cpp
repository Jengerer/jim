#include "RoundedRectangle.h"

RoundedRectangle::RoundedRectangle( int width, int height, int radius, D3DCOLOR colour )
{
	UnsetTexture();
	SetStroke( 0, D3DCOLOR_XRGB( 0, 0, 0 ) );
	SetStrokeType( STROKE_TYPE_OUTER );
	SetCornerRadius( radius, radius, radius, radius );
	SetColour( colour );
	SetSize( width, height );
}

RoundedRectangle::RoundedRectangle( int width, int height, int radiusTl, int radiusTr, int radiusBr, int radiusBl, D3DCOLOR colour )
{
	UnsetTexture();
	SetStroke( 0, 0 );
	SetCornerRadius( radiusTl, radiusTr, radiusBr, radiusBl );
	SetColour( colour );
	SetSize( width, height );
}

RoundedRectangle::~RoundedRectangle( void )
{
	RemoveTexture();
}

void RoundedRectangle::OnDraw( DirectX *directX )
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

void RoundedRectangle::SetCornerRadius( int radiusTl, int radiusTr, int radiusBr, int radiusBl )
{
	radiusTl_ = radiusTl;
	radiusTr_ = radiusTr;
	radiusBr_ = radiusBr;
	radiusBl_ = radiusBl;
}

void RoundedRectangle::Generate( DirectX *directX )
{
	int rectWidth = GetWidth();
	int rectHeight = GetHeight();
	if (GetStrokeType() == STROKE_TYPE_OUTER) {
		rectWidth += GetStrokeSize() << 1;
		rectHeight += GetStrokeSize() << 1;
	}
	roundedRect_ = directX->CreateTexture( "rounded_rect", rectWidth, rectHeight );
	directX->SetRenderTarget( roundedRect_ );
		
	// Draw stroke, if exists.
	EStrokeType strokeType = GetStrokeType();
	int strokeSize = GetStrokeSize();
	if (GetStrokeSize() != 0) {
		switch (strokeType) {
		case STROKE_TYPE_OUTER:
			directX->DrawRoundedRect(
				0, 0,
				GetWidth() + (strokeSize << 1),
				GetHeight() + (strokeSize << 1),
				radiusTl_ + strokeSize, radiusTr_ + strokeSize, radiusBr_ + strokeSize, radiusBl_ + strokeSize,
				GetStrokeColour() );
			break;
		case STROKE_TYPE_INNER:
			directX->DrawRoundedRect(
				0, 0,
				GetWidth(), GetHeight(),
				radiusTl_, radiusTr_, radiusBr_, radiusBl_,
				GetStrokeColour() );
			break;
		}
	}

	// Draw inner rectangle.
	switch (strokeType) {
	case STROKE_TYPE_OUTER:
		directX->DrawRoundedRect(
			strokeSize, strokeSize,
			GetWidth(),
			GetHeight(),
			radiusTl_, radiusTr_, radiusBr_, radiusBl_,
			GetColour() );
		break;
	case STROKE_TYPE_INNER:
		directX->DrawRoundedRect(
			strokeSize, strokeSize,
			GetWidth() - (strokeSize << 1),
			GetHeight() - (strokeSize << 1),
			radiusTl_ - strokeSize,
			radiusTr_ - strokeSize,
			radiusBr_ - strokeSize,
			radiusBl_ - strokeSize,
			GetColour() );
		break;
	}

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
