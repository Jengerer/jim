#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "Container.h"
#include "Font.h"
#include "HorizontalLayout.h"
#include "Image.h"
#include "Texture.h"
#include "WrappedText.h"

class Notification : public Container
{
	
public:

	Notification( const string& message, Texture *texture = nullptr );
	virtual ~Notification( void );

	virtual void	OnDraw( DirectX *directX );
	virtual void	Pack( void );
	virtual void	UpdatePosition( void );

	void			SetTexture( Texture *texture );
	void			SetMessage( const string& message );

	static void		Precache( DirectX *directX );
	static void		Release( void );

protected:

	static Font		*font_;

private:

	WrappedText			*text_;
	Image				*image_;
	HorizontalLayout	*layout_;

};

#endif // NOTIFICATION_H