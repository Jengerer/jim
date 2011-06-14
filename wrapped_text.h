#ifndef WRAPPED_TEXT_H
#define WRAPPED_TEXT_H

#include "Text.h"

class WrappedText : public Text
{

public:

	WrappedText( Font *font, int textWidth );
	virtual ~WrappedText( void );

	virtual void	OnDraw( DirectX *directX );
	virtual void	Pack( void );

	int				GetTextWidth( void ) const;
	void			SetTextWidth( int textWidth );

	DWORD			GetTextFormatting( void ) const;
	void			SetTextFormatting( DWORD formatting );

private:

	int		textWidth_;
	DWORD	formatting_;

};

#endif // WRAPPED_TEXT_H