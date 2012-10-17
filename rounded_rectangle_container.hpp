#ifndef ROUNDED_RECTANGLE_CONTAINER_HPP
#define ROUNDED_RECTANGLE_CONTAINER_HPP

#include <jui/layout/constrained_container.hpp>
#include <jui/layout/horizontal_layout.hpp>

#include "rounded_rectangle.hpp"

/*
 * Container with rounded background.
 */
class RoundedRectangleContainer : public JUI::ConstrainedContainer
{

public:

    RoundedRectangleContainer( unsigned int padding );
	RoundedRectangleContainer( unsigned int padding, int x, int y );
    virtual ~RoundedRectangleContainer( void );

	// Element creation.
	bool initialize( void );

	// Container functions.
	virtual void pack( void );

	// Content and rectangle.
	void				set_content( Component* content );
	RoundedRectangle*	get_rounded_rectangle( void ) const;

	// Attribute function settings.
	void set_padding( unsigned int padding );

private:

	RoundedRectangle* rounded_rect_;
	Component* content_;
	unsigned int padding_;

};

#endif // ROUNDED_RECTANGLE_CONTAINER_HPP