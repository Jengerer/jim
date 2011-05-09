#pragma once

#include <vector>

#include "Popup.h"
#include "Option.h"

#define MENU_PADDING	10
#define MENU_SPACING	10
#define MENU_RADIUS		3
#define MENU_STROKE		4
#define MENU_STROKE_COLOUR		D3DCOLOR_ARGB( 100, 153, 142, 121 )
#define MENU_BACKGROUND_COLOUR	D3DCOLOR_XRGB( 42, 39, 37 )

class Menu: public Popup, public IMouseHandler {
public:
	Menu();
	virtual ~Menu();

	// Menu handling functions.
	Option*	addOption( const string& caption, Texture *texture );
	Option*	getSelected();

	// Drawing functions.
	void pack();
	void draw( DirectX *directX );
	void display( int x, int y, Container *parent );
	virtual void updatePosition();

	// Mouse handling.
	virtual bool OnMouseMoved( Mouse *mouse );
	virtual bool OnLeftClicked( Mouse *mouse );
	virtual bool OnLeftReleased( Mouse *mouse );
	virtual bool OnRightClicked( Mouse *mouse );
	virtual bool OnRightReleased( Mouse *mouse );

private:
	Option*			selected_;
	int				widest_;
	vector<Option*>	options_;
};