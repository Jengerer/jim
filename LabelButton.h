#ifndef LABEL_BUTTON_H
#define LABEL_BUTTON_H

#include "IconButton.h"
#include "IPrecachable.h"
#include "Text.h"

class LabelButton : public IconButton
{
	
public:

	LabelButton( const string& label, Texture *icon, float x = 0.0f, float y = 0.0f );
	virtual ~LabelButton( void );

	virtual void	Pack( void );

	// Caption manipulation.
	void			SetLabel( const string& caption );

	// Static loaders/releasers.
	static void		Precache( DirectX *directX );
	static void		Release( void );

	// Static creator.
	static LabelButton* Create( const string& label, Texture *icon = nullptr, float x = 0.0f, float y = 0.0f );

public:

	static Font		*font_;

private:

	Text			*label_;
};

#endif // LABEL_BUTTON_H
