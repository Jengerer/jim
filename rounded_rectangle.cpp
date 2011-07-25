#include "rounded_rectangle.h"

RoundedRectangle::RoundedRectangle( int width, int height, int radius, const Colour& colour )
{
	UnsetTexture();
	SetStroke( 0, colour );
	SetStrokeType( STROKE_TYPE_OUTER );
	SetCornerRadius( radius );
	SetColour( colour );
	SetSize( width, height );
}

RoundedRectangle::~RoundedRectangle( void )
{
	RemoveTexture();
}

void RoundedRectangle::Draw( Graphics2D* graphics )
{
	// Check if we need to redraw.
	if (roundedRect_ == nullptr) {
		Generate( graphics );
	}

	// Adjust position for stroke.
	float textureX = GetX();
	float textureY = GetY();
	if (GetStrokeType() == STROKE_TYPE_OUTER) {
		int strokeSize = GetStrokeSize();
		textureX -= strokeSize;
		textureY -= strokeSize;
	}

	// Draw texture.
	graphics->draw_texture(
		roundedRect_,
		textureX, textureY );
}

void RoundedRectangle::SetStroke( int size, const Colour& colour )
{
	strokeSize_ = size;
	strokeColour_ = colour;
}

void RoundedRectangle::SetColour( const Colour& colour )
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

void RoundedRectangle::Generate( Graphics2D* graphics )
{
	int rectWidth = GetWidth();
	int rectHeight = GetHeight();
	if (GetStrokeType() == STROKE_TYPE_OUTER) {
		rectWidth += GetStrokeSize() << 1;
		rectHeight += GetStrokeSize() << 1;
	}

	roundedRect_ = graphics->create_empty_texture( rectWidth, rectHeight, GL_LUMINANCE_ALPHA );
	graphics->render_to_texture( roundedRect_ );

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
		graphics->set_colour( strokeColour_, GetAlpha() );
		graphics->draw_rounded_rect(	 0.0f, 0.0f, rectWidth, rectHeight, radius_ );
	}

	// Empty inner area.
	graphics->set_blend_state( GL_ZERO, GL_ONE_MINUS_SRC_ALPHA );
	graphics->set_colour( COLOUR_WHITE );
	graphics->draw_rounded_rect(
		strokeSize, strokeSize,
		rectWidth - strokeSize * 2, rectHeight - strokeSize * 2,
		innerRadius );
	graphics->set_blend_state( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Draw inner rectangle.
	graphics->set_colour( colour_, GetAlpha() );
	graphics->draw_rounded_rect(
		strokeSize, strokeSize,
		rectWidth - strokeSize * 2, rectHeight - strokeSize * 2,
		innerRadius );

	// Draw to backbuffer.
	graphics->reset_render_target();
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

const Colour& RoundedRectangle::GetColour( void ) const
{
	return colour_;
}

const Colour& RoundedRectangle::GetStrokeColour( void ) const
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
