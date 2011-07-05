#ifndef TEXT_H
#define TEXT_H

#include "component.h"
#include "font.h"

class Text : public Component//, public IGeneratable
{

public:

	Text( Font *font );
	virtual ~Text( void );

	virtual void	Draw( DirectX *directX );

	void			SetText( const string& text );
	const string&	GetText( void ) const;
	void			SetColour( D3DCOLOR colour );
	D3DCOLOR		GetColour( void ) const;

	// Drawing functions.
	Font*			GetFont( void ) const;
	void			SetFont( Font *font );
	virtual void	Pack( void );

protected:

	Font		*font_;
	string		text_;

private:

	D3DCOLOR	colour_;

};

#endif // TEXT_H