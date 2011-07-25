#ifndef TEXT_H
#define TEXT_H

#include "component.h"
#include "font.h"

class Text : public Component//, public IGeneratable
{

public:

	Text( Font *font );
	virtual ~Text( void );

	virtual void	Draw( Graphics2D* graphics );

	void			SetText( const string& text );
	const string&	GetText( void ) const;
	void			SetColour( const Colour& colour );
	const Colour&	GetColour( void ) const;

	// Drawing functions.
	Font*			GetFont( void ) const;
	void			SetFont( Font *font );
	virtual void	Pack( void );

protected:

	Font		*font_;
	string		text_;
	GLuint		list_;

private:

	Colour colour_;

};

#endif // TEXT_H