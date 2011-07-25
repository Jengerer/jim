#pragma once

#include "component.h"

class Image: public Component
{

public:

	Image( Texture* texture, float localX = 0.0f, float localY = 0.0f );

	virtual void	Draw( Graphics2D* graphics );
	void			SetTexture( Texture *texture );

	void			SetAlpha( int alpha );
	void			SetTint( const Colour& tint );
	const Colour&	GetTint( void ) const;

private:

	Texture*		GetTexture( void ) const;

private:

	Texture *texture_;
	Colour tint_;

};