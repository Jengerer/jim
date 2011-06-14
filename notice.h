#ifndef NOTICE_H
#define NOTICE_H

#include "IDrawable.h"
#include "Popup.h"

#define NOTICE_WIDTH			300
#define NOTICE_RADIUS			10
#define NOTICE_PADDING			20
#define NOTICE_STROKE_WIDTH		5
#define NOTICE_STROKE_COLOUR	D3DCOLOR_RGBA( 153, 142, 121, 100 )
#define NOTICE_COLOUR			D3DCOLOR_XRGB( 42, 39, 37 )

class Notice : public Popup
{

public:

	Notice( const string& message );
	virtual ~Notice( void );

	virtual void	OnDraw( DirectX* directX );

	// Resizes the notification based on new text.
	virtual void	Pack( void );

	const string*	GetMessage( void ) const;
	void			SetMessage( const string& message );
	void			AppendMessage( const string& message );

	static void		Precache( DirectX *directX );
	static void		Release( void );

protected:

	// Class-wide texture and font.
	static Font		*font_;

private:

	string message_;

};

#endif // NOTICE_H