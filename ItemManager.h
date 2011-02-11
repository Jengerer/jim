#pragma once

#include <vector>
#include <deque>
#include <sstream>
#include <string>

// Parsing APIs.
#include <json/json.h>
#include <boost/regex.hpp>

// Drawing API.
#include "DirectX.h"

// User interface.
#include "Button.h"
#include "Popup.h"
#include "Dialog.h"
#include "Alert.h"
#include "Mouse.h"

// Items and definitions.
#include "Hashtable.h"
#include "Backpack.h"
#include "SerializedBuffer.h"

#define BUTTON_SPACING	10
#define BUTTON_Y		380

class ItemManager: public DirectX
{
public:
	ItemManager( HINSTANCE hInstance );
	virtual ~ItemManager();

	// Initializing and closing.
	void openInterfaces();
	void closeInterfaces();

	// Item defining and loading.
	void loadDefinitions();
	void loadItems();

	// Drawing functions.
	void run();
	void onRedraw();

	// Steam handling.
	void handleCallbacks();

	// Input message handling.
	void handleMouse();
	void handleKeyboard();

	// Mouse handling virtuals.
	virtual bool mouseClicked( Mouse *mouse );
	virtual bool mouseReleased( Mouse *mouse );
	virtual bool mouseMoved( Mouse *mouse );

	// Interface handling.
	Dialog*	createDialog( const string& message );
	Alert*	createAlert( const string& message );
	Button*	createButton( const string& caption, Texture *texture = 0, float x = 0.0f, float y = 0.0f, EAlignment align = ALIGN_TOP_LEFT );
	void	showPopup(Popup* popup);
	void	removePopup(Popup* popup);

private:
	// Application interfaces.
	Backpack*		backpack_;

	// User interface stacks.
	deque<Popup*>	popupStack_;
	vector<Popup*>	popupList_;
	vector<Button*>	buttonList_;

	// Input handling.
	Mouse*			mouse_;

	// Keyboard booleans.
	bool			leftPressed_;
	bool			rightPressed_;
	bool			enterPressed_;

	// Interface variables.
	Dialog			*loadDialog_;
	Alert			*alert_;
	Alert			*error_;
	Button			*equipButton_, *craftButton_, *sortButton_;
};