#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "container.h"
#include "font.h"
#include "horizontal_layout.h"
#include "image.h"
#include "rounded_container.h"
#include "texture.h"
#include "wrapped_text.h"

class Notification : public RoundedRectangleContainer
{
	
public:

	Notification( const std::string& message, const Texture *texture = nullptr );
	virtual ~Notification( void );

	virtual void	Pack( void );
	void			SetMessage( const std::string& message );

	static void		Precache();
	static void		Release();

protected:

	static Font		*font_;

private:

	WrappedText			*text_;
	Image				*image_;
	HorizontalLayout	*layout_;
	RoundedRectangle	*roundedRect_;

};

#endif // NOTIFICATION_H