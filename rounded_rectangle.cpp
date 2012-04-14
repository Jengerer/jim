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
	// Translate to position.
	graphics->push_matrix();
	graphics->translate( get_x(), get_y() );

	// Get starting position.
	int x = 0;
	int y = 0;
	int radius = get_radius();
	int width = get_width();
	int height = get_height();
	if (get_stroke_type() == STROKE_TYPE_OUTER) {
		x -= get_stroke_size();
		y -= get_stroke_size();
		radius += get_stroke_size();
		width += get_stroke_size() * 2;
		height += get_stroke_size() * 2;
	}

	// Draw outer stroke.
	if (get_stroke_size() != 0) {
		// Get average of colour.
		float stroke_alpha = static_cast<float>(get_alpha()) * static_cast<float>(stroke_colour_.a) / (255.0f);
		graphics->set_colour( stroke_colour_.r, stroke_colour_.g, stroke_colour_.b, static_cast<GLubyte>(stroke_alpha) );
		graphics->draw_rounded_rectangle(
			x, y,
			width, height,
			radius );

		x += get_stroke_size();
		y += get_stroke_size();
		radius -= get_stroke_size();
		width -= get_stroke_size() * 2;
		height -= get_stroke_size() * 2;

		// Empty inner area.
		graphics->set_blend_state( GL_ZERO, GL_ONE_MINUS_SRC_ALPHA );
		graphics->set_colour( COLOUR_WHITE );
		graphics->draw_rounded_rectangle(
			x, y,
			width, height,
			radius );
		graphics->set_blend_state( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}

	// Draw inner.
	float fill_alpha = static_cast<float>(get_alpha()) * static_cast<float>(colour_.a) / (255.0f);
	graphics->set_colour( colour_.r, colour_.g, colour_.b, fill_alpha );
	graphics->draw_rounded_rectangle(
		x, y,
		width, height,
		radius );

	// Pop translation.
	graphics->pop_matrix();
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

int RoundedRectangle::get_radius() const
{
	return radius_;
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
