#pragma once

#include "Component.h"

class Image: public Component
{

public:

	Image( float x, float y, Texture *texture = nullptr );
	virtual ~Image( void );

	virtual void	OnDraw( DirectX *directX );
	void			SetTexture( Texture *texture );

private:

	Texture*		GetTexture( void ) const;

private:

	Texture *texture_;

};