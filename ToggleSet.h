#ifndef TOGGLE_SET_H
#define TOGGLE_SET_H

#include <vector>

#include "Button.h"
#include "Container.h"
#include "Layout.h"
#include "RoundedRectangle.h"
#include "Text.h"

class ToggleSet : public Container, public IMouseHandler
{

public:

	ToggleSet( const string& nameSetA, const string& nameSetB );
	virtual ~ToggleSet( void );

	void	AddSetA( Button *button );
	void	AddSetB( Button *button );

	vector<Button*>*	GetSetA( void ) const;
	vector<Button*>*	GetSetB( void ) const;

	virtual bool OnMouseMoved( Mouse *mouse );
	virtual bool OnLeftClicked( Mouse *mouse );
	virtual bool OnLeftReleased( Mouse *mouse );

private:

	Text	*titleSetA_;
	Text	*titleSetB_;

	VerticalLayout *layoutSetA_;
	VerticalLayout *layoutSetB_;

	vector<Button*> buttonSetA_;
	vector<Button*> buttonSetB_;

};

#endif // TOGGLE_SET_H