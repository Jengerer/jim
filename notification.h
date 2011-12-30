#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <jui/container.h>
#include <jui/ifont.h>
#include <jui/font_factory.h>
#include <jui/horizontal_layout.h>
#include <jui/image.h>
#include <jui/texture.h>
#include <jui/wrapped_text.h>

#include "rounded_container.h"

class Notification : public RoundedRectangleContainer
{
	
public:

	Notification( const std::string& message, const Texture *texture = nullptr );
	virtual ~Notification( void );

	virtual void	pack( void );
	void			set_message( const std::string& message );

	static void		precache();
	static void		release();

protected:

	static IFont		*font_;

private:

	WrappedText			*text_;
	Image				*image_;
	HorizontalLayout	*layout_;
	RoundedRectangle	*roundedRect_;

};

#endif // NOTIFICATION_H