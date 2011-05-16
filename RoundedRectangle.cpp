#include "RoundedRectangle.h"

RoundedRectangle::RoundedRectangle( int width, int height, int radius, D3DCOLOR colour )
{
	UnsetTexture();
	SetStroke( 0, D3DCOLOR_XRGB( 0, 0, 0 ) );
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
	directX->DrawTexture(
		roundedRect_,
		GetX(), GetY(),
		GetWidth(), GetHeight(),
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
	roundedRect_ = directX->CreateTexture( "rounded_rect", GetWidth(), GetHeight() );
	directX->SetRenderTarget( roundedRect_ );
		
	// Draw stroke, if exists.
	if (GetStrokeSize() != 0) {
		directX->DrawRoundedRect(
			0, 0,
			GetWidth(), GetHeight(),
			radiusTl_, radiusTr_, radiusBr_, radiusBl_,
			GetStrokeColour() );
	}

	// Draw inner rectangle.
	directX->DrawRoundedRect(
		GetStrokeSize(),
		GetStrokeSize(),
		GetWidth() - (GetStrokeSize() << 1),
		GetHeight() - (GetStrokeSize() << 1),
		radiusTl_ - GetStrokeSize(),
		radiusTr_ - GetStrokeSize(),
		radiusBr_ - GetStrokeSize(),
		radiusBl_ - GetStrokeSize(),
		GetColour() );

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