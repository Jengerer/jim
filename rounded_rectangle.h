#ifndef ROUNDED_RECTANGLE_H
#define ROUNDED_RECTANGLE_H

#include "component.h"

enum EStrokeType {
	STROKE_TYPE_OUTER,
	STROKE_TYPE_INNER,
	// STROKE_TYPE_MIDDLE
};

class RoundedRectangle : public Component
{

public:

	RoundedRectangle( int width, int height, int radius, const Colour& colour );
	virtual ~RoundedRectangle( void );

	virtual void	draw( Graphics2D* graphics );

	void			SetStroke( int size, const Colour& colour );
	void			SetStrokeType( EStrokeType strokeType );
	const Colour&	GetColour( void ) const;
	void			SetColour( const Colour& colour );
	virtual void	set_size( int width, int height );
	void			SetCornerRadius( int radius );

	// TODO: Maybe make Generate protected, but a friend of IPrecachable.
	void			Generate( Graphics2D* graphics );
	Texture*		get_texture( void ) const;
	void			RemoveTexture( void );

private:

	// Handling texture.
	void		UnsetTexture( void );

	// Private getters.
	const Colour& GetStrokeColour( void ) const;
	int			GetStrokeSize( void ) const;
	EStrokeType	GetStrokeType( void ) const;

private:

	int			radius_;

	Colour		colour_;
	Colour		strokeColour_;
	int			strokeSize_;
	EStrokeType	strokeType_;

	Texture		*roundedRect_;

};

#endif // ROUNDED_RECTANGLE_H