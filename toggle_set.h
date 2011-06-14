#ifndef TOGGLE_SET_H
#define TOGGLE_SET_H

#include <vector>

#include "button.h"
#include "font.h"
#include "grid_layout.h"
#include "horizontal_layout.h"
#include "label_button.h"
#include "layout.h"
#include "popup.h"
#include "rounded_rectangle.h"
#include "text.h"
#include "vertical_layout.h"

typedef vector<Button*> ButtonList;

class ToggleSet : public Popup, public IMouseHandler
{

public:

	ToggleSet( const string& nameSetA, const string& nameSetB, float x = 0.0f, float y = 0.0f );
	virtual ~ToggleSet( void );

	virtual void Pack( void );

	void	AddSetA( Button *button );
	void	AddSetB( Button *button );

	void	RemoveSetA( Button *button );
	void	RemoveSetB( Button *button );

	bool	InSetA( Button *button ) const;
	bool	InSetB( Button *button ) const;

	virtual bool OnMouseMoved( Mouse *mouse );
	virtual bool OnLeftClicked( Mouse *mouse );
	virtual bool OnLeftReleased( Mouse *mouse );

	static void Precache( DirectX *directX );
	static void Release( void );

private:

	bool InSet( const ButtonList* set, Button *button ) const;
	void RemoveSet( ButtonList* set, Button *button );

protected:

	static Font *titleFont_;

private:

	Text	*titleSetA_;
	Text	*titleSetB_;

	Button			*okayButton_;
	Button			*cancelButton_;

	VerticalLayout	*setLayout_;
	Layout			*layoutSetA_;
	Layout			*layoutSetB_;
	Layout			*buttonLayout_;

	RoundedRectangle *roundedRect_;

	ButtonList buttonSetA_;
	ButtonList buttonSetB_;

};

#endif // TOGGLE_SET_H