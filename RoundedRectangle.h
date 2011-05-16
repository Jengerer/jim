#ifndef ROUNDED_RECTANGLE_H
#define ROUNDED_RECTANGLE_H

#include "Container.h"
#include "DirectX.h"

class RoundedRectangle : public Container
{

public:

	RoundedRectangle( int width, int height, int radius, D3DCOLOR colour );
	RoundedRectangle( int width, int height, int radiusTl, int radiusTr, int radiusBr, int radiusBl, D3DCOLOR colour );
	virtual ~RoundedRectangle( void );

	virtual void	OnDraw( DirectX *directX );

	void			SetStroke( int size, D3DCOLOR colour );
	void			SetColour( D3DCOLOR colour );
	virtual void	SetSize( int width, int height );
	void			SetCornerRadius( int radiusTl, int radiusTr, int radiusBr, int radiusBl );

	// TODO: Maybe make Generate protected, but a friend of IPrecachable.
	void			Generate( DirectX *directX );
	Texture*		GetTexture( void ) const;
	void			RemoveTexture( void );

private:

	// Handling texture.
	void		UnsetTexture( void );

	// Private getters.
	D3DCOLOR	GetColour( void ) const;
	D3DCOLOR	GetStrokeColour( void ) const;
	int			GetStrokeSize( void ) const;

private:

	int			radiusTl_;
	int			radiusTr_;
	int			radiusBr_;
	int			radiusBl_;

	D3DCOLOR	colour_;
	D3DCOLOR	strokeColour_;
	int			strokeSize_;

	Texture		*roundedRect_;

};

#endif // ROUNDED_RECTANGLE_H