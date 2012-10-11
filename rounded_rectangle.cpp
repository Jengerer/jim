#include "rounded_rectangle.hpp"
#include <string/string.hpp>

// Static texture.
const JUTIL::ConstantString ROUNDED_CORNER_TEXTURE = "img/manager/rounded_corner.png";
JUI::FileTexture* RoundedRectangle::rounded_corner_ = nullptr;

/*
 * Rounded rectangle constructor.
 */
RoundedRectangle::RoundedRectangle( int width, int height, int radius, const JUI::Colour* colour )
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
bool RoundedRectangle::precache( JUI::Graphics2D* graphics )
{
    // Load rounded corner texture.
    // TODO: Replace with rounded rectangle pixel-render.
	if (!graphics->get_texture( &ROUNDED_CORNER_TEXTURE, &rounded_corner_ ))
    {
        return false;
    }

    return true;
}

/*
 * Draw rounded rectangle.
 */
void RoundedRectangle::draw( JUI::Graphics2D* graphics )
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
        JUI::Colour colour( stroke_colour_ );
		float stroke_alpha = static_cast<float>(get_alpha()) * static_cast<float>(stroke_colour_.a) / (255.0f);
        colour_.a = static_cast<GLubyte>(stroke_alpha);
		graphics->set_colour( colour );
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
		graphics->set_colour( JUI::COLOUR_WHITE );
		graphics->draw_rounded_rectangle(
			x, y,
			width, height,
			radius );
		graphics->set_blend_state( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}

	// Draw inner.
    JUI::Colour colour = colour_;
	float fill_alpha = static_cast<float>(get_alpha()) * static_cast<float>(colour_.a) / (255.0f);
    colour_.a = static_cast<GLubyte>(fill_alpha);
	graphics->set_colour( colour );
	graphics->draw_rounded_rectangle(
		x, y,
		width, height,
		radius );

	// Pop translation.
	graphics->pop_matrix();
}

/*
 * Draw rounded rectangle from corners and filling.
 */
void RoundedRectangle::draw_rounded_rectangle( JUI::Graphics2D* graphics, float x, float y, float width, float height, float radius )
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

/*
 * Set the stroke colour for the rounded rectangle.
 */
void RoundedRectangle::set_stroke( int size, const JUI::Colour* colour )
{
	stroke_size_ = size;
	stroke_colour_ = *colour;
}

/*
 * Set the fill colour for the rectangle.
 */
void RoundedRectangle::set_colour( const JUI::Colour* colour )
{
	colour_ = *colour;
}

/*
 * Set the rounded corner radius.
 */
void RoundedRectangle::set_radius( int radius )
{
	radius_ = radius;
}

/*
 * Get the rounded corner radius.
 */
int RoundedRectangle::get_radius( void ) const
{
	return radius_;
}

/*
 * Get the fill colour.
 */
const JUI::Colour* RoundedRectangle::get_colour( void ) const
{
	return &colour_;
}

/*
 * Get the stroke colour.
 */
const JUI::Colour* RoundedRectangle::get_stroke_colour( void ) const
{
	return &stroke_colour_;
}

/*
 * Get the thickness of the stroke.
 */
int RoundedRectangle::get_stroke_size( void ) const
{
	return stroke_size_;
}

/*
 * Set the stroke type to be rendered.
 */
void RoundedRectangle::set_stroke_type( EStrokeType stroke_type )
{
	stroke_type_ = stroke_type;
}

/*
 * Get the stroke type setting.
 */
EStrokeType RoundedRectangle::get_stroke_type( void ) const
{
	return stroke_type_;
}
