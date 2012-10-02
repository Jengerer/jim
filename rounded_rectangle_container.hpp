#ifndef ROUNDED_RECTANGLE_CONTAINER_H
#define ROUNDED_RECTANGLE_CONTAINER_H

#include <jui/layout/constrained_container.hpp>
#include <jui/layout/horizontal_layout.hpp>

#include "rounded_rectangle.hpp"

/*
 * Container with rounded background.
 */
class RoundedRectangleContainer : public JUI::ConstrainedContainer
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