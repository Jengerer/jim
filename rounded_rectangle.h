#ifndef ROUNDED_RECTANGLE_H
#define ROUNDED_RECTANGLE_H

#include "component.h"
#include "directx.h"

enum EStrokeType {
	STROKE_TYPE_OUTER,
	STROKE_TYPE_INNER,
	// STROKE_TYPE_MIDDLE
};

class RoundedRectangle : public Component
{

public:

	RoundedRectangle( int width, int height, int radius, D3DCOLOR colour );
	virtual ~RoundedRectangle( void );

	virtual void	OnDraw( DirectX *directX );

	void			SetStroke( int size, D3DCOLOR colour );
	void			SetStrokeType( EStrokeType strokeType );
	D3DCOLOR		GetColour( void ) const;
	void			SetColour( D3DCOLOR colour );
	virtual void	SetSize( int width, int height );
	void			SetCornerRadius( int radius );

	// TODO: Maybe make Generate protected, but a friend of IPrecachable.
	void			Generate( DirectX *directX );
	Texture*		GetTexture( void ) const;
	void			RemoveTexture( void );

private:

	// Handling texture.
	void		UnsetTexture( void );

	// Private getters.
	D3DCOLOR	GetStrokeColour( void ) const;
	int			GetStrokeSize( void ) const;
	EStrokeType	GetStrokeType( void ) const;

private:

	int			radius_;

	D3DCOLOR	colour_;
	D3DCOLOR	strokeColour_;
	int			strokeSize_;
	EStrokeType	strokeType_;

	Texture		*roundedRect_;

};

#endif // ROUNDED_RECTANGLE_H