#ifndef ROUNDED_RECTANGLE_CONTAINER_H
#define ROUNDED_RECTANGLE_CONTAINER_H

#include "Container.h"
#include "RoundedRectangle.h"

// A new record! Longest class name in the project!
class RoundedRectangleContainer : public Container
{

public:

	RoundedRectangleContainer( unsigned int radius, float x = 0.0f, float y = 0.0f );
	virtual ~RoundedRectangleContainer( void );

	virtual void		Pack( void );
	virtual void		UpdatePosition( void );

	// Container setting functions.
	void				SetContained( Component *component );

	// RoundedRectangle settings.
	void				SetColour( D3DCOLOR colour );
	void				SetStroke( unsigned int size, D3DCOLOR colour );
	void				SetStrokeType( EStrokeType strokeType );
	void				SetRadius( unsigned int radius );

	// Attribute function settings.
	void				SetPadding( unsigned int padding );

private:

	Component*			GetContained( void ) const;
	unsigned int		GetPadding( void ) const;

private:

	RoundedRectangle	*roundedRect_;
	Component			*contained_;
	unsigned int		padding_;

};

#endif // ROUNDED_RECTANGLE_CONTAINER_H