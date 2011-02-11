#pragma once

#include <vector>

#include "Popup.h"
#include "MenuOption.h"

#define MENU_PADDING	10
#define MENU_SPACING	10
#define MENU_RADIUS		3
#define MENU_STROKE		3
#define MENU_STROKE_COLOUR		D3DCOLOR_ARGB( 100, 153, 142, 121 )
#define MENU_BACKGROUND_COLOUR	D3DCOLOR_XRGB( 42, 39, 37 )

class Menu: public Popup {
public:
	Menu();
	virtual ~Menu();

	// Menu handling functions.
	MenuOption*	addOption( const string& caption );
	MenuOption*	getSelected();

	// Drawing functions.
	void pack();
	void draw( DirectX *directX );
	virtual void updatePosition();

	// Mouse handling.
	virtual bool leftClicked( Mouse *mouse );
	virtual bool leftReleased( Mouse *mouse );
	virtual bool rightClicked( Mouse *mouse );
	virtual bool mouseMoved( Mouse *mouse );

private:
	MenuOption*			selected_;
	int					widest_;
	vector<MenuOption*>	options_;
};