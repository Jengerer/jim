#ifndef ROUNDED_RECTANGLE_CONTAINER_H
#define ROUNDED_RECTANGLE_CONTAINER_H

#include "container.h"
#include "horizontal_layout.h"
#include "rounded_rectangle.h"

// A new record! Longest class name in the project!
class RoundedRectangleContainer : public Container
{

public:

	RoundedRectangleContainer( unsigned int radius, unsigned int padding, float localX = 0.0f, float localY = 0.0f );

	// Container functions.
	virtual void		Pack();

	// Content and rectangle.
	void				SetContent( Component* content );
	RoundedRectangle*	GetRoundedRectangle() const;

	// Attribute function settings.
	void				SetPadding( unsigned int padding );

private:

	RoundedRectangle*	roundedRect_;
	Component*			content_;
	unsigned int		padding_;

};

#endif // ROUNDED_RECTANGLE_CONTAINER_H