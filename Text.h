#ifndef TEXT_H
#define TEXT_H

#include "Component.h"
#include "Font.h"

class Text : public Component
{

public:

	Text( Font *font );
	virtual ~Text( void );

	void			SetText( const string& text );
	const string&	GetText( void ) const;
	void			SetColour( D3DCOLOR colour );
	D3DCOLOR		GetColour( void ) const;

	// Drawing functions.
	virtual void	OnDraw( DirectX *directX );
	void			SetFont( Font *font );
	virtual void	Pack( void );

private:

	Font		*font_;
	string		text_;
	D3DCOLOR	colour_;

};

#endif // TEXT_H