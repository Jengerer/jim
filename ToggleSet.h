#ifndef TOGGLE_SET_H
#define TOGGLE_SET_H

#include <vector>

#include "Button.h"
#include "Font.h"
#include "HorizontalLayout.h"
#include "Layout.h"
#include "Popup.h"
#include "RoundedRectangle.h"
#include "Text.h"
#include "VerticalLayout.h"

typedef vector<Button*> ButtonList;

class ToggleSet : public Popup, public IMouseHandler
{

public:

	ToggleSet( const string& nameSetA, const string& nameSetB );
	virtual ~ToggleSet( void );

	virtual void Pack( void );
	virtual void UpdatePosition( void );

	void	AddSetA( Button *button );
	void	AddSetB( Button *button );

	void	RemoveSetA( Button *button );
	void	RemoveSetB( Button *button );

	const ButtonList*	GetSetA( void ) const;
	const ButtonList*	GetSetB( void ) const;

	virtual bool OnMouseMoved( Mouse *mouse );
	virtual bool OnLeftClicked( Mouse *mouse );
	virtual bool OnLeftReleased( Mouse *mouse );

	static void Precache( DirectX *directX );
	static void Release( void );

private:

	void RemoveSet( ButtonList* set, Button *button );

protected:

	static Font *titleFont_;

private:

	Text	*titleSetA_;
	Text	*titleSetB_;

	HorizontalLayout	*setLayout_;
	VerticalLayout		*layoutSetA_;
	VerticalLayout		*layoutSetB_;

	RoundedRectangle *roundedRect_;

	ButtonList buttonSetA_;
	ButtonList buttonSetB_;

};

#endif // TOGGLE_SET_H