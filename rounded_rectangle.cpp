#include "rounded_rectangle.h"

// Static texture.
FileTexture* RoundedRectangle::rounded_corner_ = nullptr;

/*
 * Rounded rectangle constructor.
 */
RoundedRectangle::RoundedRectangle( int width, int height, int radius, const Colour& colour )
{
	set_stroke( 0, colour );
	set_stroke_type( STROKE_TYPE_OUTER );
	set_radius( radius );
	set_colour( colour );
	set_size( width, height );
}

/*
 * Rounded rectangle destructor.
 */
RoundedRectangle::~RoundedRectangle( void )
{
	// Nothing.
}

/*
 * Precaching texture.
 */
void RoundedRectangle::precache( Graphics2D* graphics )
{
	rounded_corner_ = graphics->get_texture( "img/manager/rounded_corner.png" );
}

/*
 * Draw rounded rectangle.
 */
void RoundedRectangle::draw( Graphics2D* graphics )
{
	glPushMatrix();
	glTranslatef( get_x(), get_y(), 0.0f );

	int rect_width = get_width();
	int rect_height = get_height();
	if (get_stroke_type() == STROKE_TYPE_OUTER) {
		rect_width += get_stroke_size() << 1;
		rect_height += get_stroke_size() << 1;
	}

	// Adjust radius of inner and outer based on stroke type.
	int innerRadius = radius_;
	int outerRadius = radius_;
	int strokeSize = get_stroke_size();
	EStrokeType strokeType = get_stroke_type();
	if (strokeType == STROKE_TYPE_OUTER) {
		outerRadius += strokeSize;
	}
	else {
		innerRadius -= strokeSize;
	}

	// Draw outer rectangle if needed.
	if (strokeSize != 0) {
		graphics->set_colour( get_stroke_colour() );
		draw_rounded_rectangle(
			graphics,
			0.0f, 0.0f,
			static_cast<float>(rect_width),
			static_cast<float>(rect_height),
			static_cast<float>(radius_) );
	}

	// Empty inner area.
	graphics->set_blend_state( GL_ZERO, GL_ONE_MINUS_SRC_ALPHA );
	graphics->set_colour( COLOUR_WHITE );
	draw_rounded_rectangle(
		graphics,
		static_cast<GLfloat>(strokeSize), static_cast<GLfloat>(strokeSize),
		static_cast<GLfloat>(rect_width - strokeSize * 2),
		static_cast<GLfloat>(rect_height - strokeSize * 2),
		static_cast<GLfloat>(innerRadius) );
	graphics->set_blend_state( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Draw inner rectangle.
	graphics->set_colour( colour_ );
	draw_rounded_rectangle(
		graphics,
		static_cast<float>(strokeSize),
		static_cast<float>(strokeSize),
		static_cast<float>(rect_width - strokeSize * 2), 
		static_cast<float>(rect_height - strokeSize * 2),
		static_cast<float>(innerRadius) );

	glPopMatrix();
}

void RoundedRectangle::draw_rounded_rectangle( Graphics2D* graphics, float x, float y, float width, float height, float radius )
{
	GLsizei size_radius = static_cast<GLsizei>(radius);

	// Corners.
	graphics->draw_texture( rounded_corner_, x, y, size_radius, size_radius ); // Top-left.
	graphics->draw_texture( rounded_corner_, x + width, y, -size_radius, size_radius ); // Top-right.
	graphics->draw_texture( rounded_corner_, x + width, y + height, -size_radius, -size_radius ); // Bottom-right.
	graphics->draw_texture( rounded_corner_, x, y + height, size_radius, -size_radius ); // Bottom-left.

	// Double radius.
	GLfloat double_radius = radius * 2.0f;

	// Fill in center and tops.
	graphics->draw_rectangle( x, y + radius, width, height - double_radius ); // Middle.
	graphics->draw_rectangle( x + radius, y, width - double_radius, radius ); // Top.
	graphics->draw_rectangle( x + radius, y + height - radius, width - double_radius, radius ); // Bottom.
}

void RoundedRectangle::set_stroke( int size, const Colour& colour )
{
	stroke_size_ = size;
	stroke_colour_ = colour;
}

void RoundedRectangle::set_colour( const Colour& colour )
{
	colour_ = colour;
}

void RoundedRectangle::set_radius( int radius )
{
	radius_ = radius;
}

const Colour& RoundedRectangle::get_colour( void ) const
{
	return colour_;
}

const Colour& RoundedRectangle::get_stroke_colour( void ) const
{
	return stroke_colour_;
}

int RoundedRectangle::get_stroke_size( void ) const
{
	return stroke_size_;
}

void RoundedRectangle::set_stroke_type( EStrokeType stroke_type )
{
	stroke_type_ = stroke_type;
}

EStrokeType RoundedRectangle::get_stroke_type( void ) const
{
	return stroke_type_;
}
