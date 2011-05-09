#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "IDrawable.h"
#include "Popup.h"

#define NOTIFICATION_WIDTH			300
#define NOTIFICATION_RADIUS			10
#define NOTIFICATION_PADDING		20
#define NOTIFICATION_STROKE_WIDTH	5
#define NOTIFICATION_STROKE_COLOUR	D3DCOLOR_RGBA( 153, 142, 121, 100 )
#define NOTIFICATION_COLOUR			D3DCOLOR_XRGB( 42, 39, 37 )

class Notification : public Popup
{

public:

	Notification( const string& message );
	virtual ~Notification( void );

	void			OnDraw( DirectX* directX );

	// Resizes the notification based on new text.
	virtual void	Pack( void );

	const string*	GetMessage( void ) const;
	void			SetMessage( const string& message );
	void			AppendMessage( const string& message );

	// Class-wide texture and font.
	static Texture	*texture;
	static Font		*font;

private:

	string message_;

};

#endif // NOTIFICATION_H