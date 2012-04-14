#ifndef ROUNDED_RECTANGLE_CONTAINER_H
#define ROUNDED_RECTANGLE_CONTAINER_H

#include <jui/constrained_container.h>
#include <jui/horizontal_layout.h>

#include "rounded_rectangle.h"

/*
 * Container with rounded background.
 */
class RoundedRectangleContainer : public ConstrainedContainer
{

public:

	RoundedRectangleContainer( unsigned int radius, unsigned int padding, float localX = 0.0f, float localY = 0.0f );

	// Container functions.
	virtual void		pack();

	// Content and rectangle.
	void				set_content( Component* content );
	RoundedRectangle*	get_rounded_rectangle() const;

	// Attribute function settings.
	void				set_padding( unsigned int padding );

private:

	RoundedRectangle*	rounded_rect_;
	Component*			content_;
	unsigned int		padding_;

};

#endif // ROUNDED_RECTANGLE_CONTAINER_H