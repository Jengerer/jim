#ifndef ROUNDED_RECTANGLE_H
#define ROUNDED_RECTANGLE_H

#include <jui/layout/component.hpp>

enum EStrokeType {
	STROKE_TYPE_OUTER,
	STROKE_TYPE_INNER,
	// STROKE_TYPE_MIDDLE
};

class RoundedRectangle : public JUI::Component
{

public:

	RoundedRectangle( int width, int height, int radius, const JUI::Colour& colour );
	virtual ~RoundedRectangle( void );

	// Static texture loader.
	static bool		precache( JUI::Graphics2D* graphics );

	// IDrawable over-ride.
	virtual void	draw( JUI::Graphics2D* graphics );

	// Drawing helper function.
	static void		draw_rounded_rectangle( JUI::Graphics2D* graphics, float x, float y, float width, float height, float radius );

	// Rounded rectangle property setters.
	void			set_radius( int radius );

	// Stroke property setters.
	void			set_stroke( int size, const JUI::Colour& colour );
	void			set_stroke_type( EStrokeType stroke_type );
	void			set_stroke( int radius );

	// JUI::Colour setter/getter.
	const JUI::Colour&	get_colour( void ) const;
	void			set_colour( const JUI::Colour& colour );

private:

	// Private rounded rectangle getters.
	int				get_radius() const;

	// Private stroke getters (hehe, private stroke).
	const JUI::Colour&	get_stroke_colour( void ) const;
	int				get_stroke_size( void ) const;
	EStrokeType		get_stroke_type( void ) const;

private:

	// Rounded corner image.
	static JUI::FileTexture* rounded_corner_;

private:

	// Corner radius.
	int			radius_;

	// Fill colour.
	JUI::Colour		colour_;

	// Stroke properties.
	JUI::Colour		stroke_colour_;
	int			stroke_size_;
	EStrokeType	stroke_type_;

};

#endif // ROUNDED_RECTANGLE_H