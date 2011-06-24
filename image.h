#pragma once

#include "component.h"

class Image: public Component
{

public:

	Image( Texture* texture, float localX = 0.0f, float localY = 0.0f );

	virtual void	OnDraw( DirectX *directX );
	void			SetTexture( Texture *texture );

private:

	Texture*		GetTexture( void ) const;

private:

	Texture *texture_;

};