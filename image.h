#pragma once

#include "component.h"

class Image: public Component
{

public:

	Image( const Texture* texture, float localX = 0.0f, float localY = 0.0f );

	virtual void	draw( Graphics2D* graphics );
	void			set_texture( const Texture *texture );

	void			set_alpha( int alpha );
	void			set_tint( const Colour& tint );
	const Colour&	get_tint( void ) const;

private:

	const Texture*	get_texture( void ) const;

private:

	const Texture *texture_;
	Colour tint_;

};