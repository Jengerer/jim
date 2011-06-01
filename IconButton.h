#ifndef ICON_BUTTON_H
#define ICON_BUTTON_H

#include "Button.h"
#include "Image.h"
#include "Texture.h"

class IconButton : public Button
{

public:

	IconButton( Texture *icon, float x = 0.0f, float y = 0.0f );
	virtual ~IconButton( void );

	void				SetIcon( Texture *icon );

	static IconButton* Create( Texture *texture, float x = 0.0f, float y = 0.0f );

protected:

	Image*			GetIcon( void ) const;

private:

	Image	*icon_;

};

#endif // ICON_BUTTON_H