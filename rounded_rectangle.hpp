#ifndef ROUNDED_RECTANGLE_HPP
#define ROUNDED_RECTANGLE_HPP

#include <jui/layout/component.hpp>

enum EStrokeType {
	STROKE_TYPE_OUTER,
	STROKE_TYPE_INNER,
	// STROKE_TYPE_MIDDLE
};

class RoundedRectangle : public JUI::Component
{

public:

	RoundedRectangle( int width, int height, int radius, const JUI::Colour* colour );
	virtual ~RoundedRectangle( void );

	// IDrawable over-ride.
	virtual void draw( JUI::Graphics2D* graphics );

	// Rounded rectangle property setters.
	void set_radius( int radius );

	// Stroke property setters.
	void set_stroke( int size, const JUI::Colour* colour );
	void set_stroke_type( EStrokeType stroke_type );

	// JUI::Colour setter/getter.
	const JUI::Colour* get_colour( void ) const;
	void set_colour( const JUI::Colour* colour );

private:

	// Private rounded rectangle getters.
	int get_radius( void ) const;

	// Private stroke getters (hehe, private stroke).
	const JUI::Colour* get_stroke_colour( void ) const;
	int get_stroke_size( void ) const;
	EStrokeType get_stroke_type( void ) const;

private:

	// Corner radius.
	int radius_;

	// Fill colour.
	JUI::Colour colour_;

	// Stroke properties.
	JUI::Colour stroke_colour_;
	int stroke_size_;
	EStrokeType stroke_type_;

};

#endif // ROUNDED_RECTANGLE_HPP