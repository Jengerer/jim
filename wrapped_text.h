#ifndef WRAPPED_TEXT_H
#define WRAPPED_TEXT_H

#include "text.h"

class WrappedText : public Text
{

public:

	WrappedText( Font *font, int textWidth );
	virtual ~WrappedText( void );

	virtual void	pack( void );

	int				get_text_width( void ) const;
	void			set_text_width( int textWidth );

	DWORD			get_text_formatting( void ) const;
	void			set_text_formatting( DWORD formatting );

private:

	int		textWidth_;
	DWORD	formatting_;

};

#endif // WRAPPED_TEXT_H