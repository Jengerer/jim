#pragma once

#include "component.h"

class Image: public Component
{

public:

	Image( Texture* texture, float localX = 0.0f, float localY = 0.0f );

	virtual void	Draw( DirectX *directX );
	void			SetTexture( Texture *texture );

	void			SetTint( D3DCOLOR tint );
	D3DCOLOR		GetTint( void ) const;

private:

	Texture*		GetTexture( void ) const;

private:

	Texture *texture_;
	D3DCOLOR tint_;

};