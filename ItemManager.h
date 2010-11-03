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
#include "ButtonListener.h"
#include "Popup.h"
#include "Dialog.h"
#include "Alert.h"

// Items and definitions.
#include "Hashtable.h"
#include "Backpack.h"
#include "SerializedBuffer.h"

class ItemManager: public DirectX, public ButtonListener
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

	// Input message transmitting.
	void triggerMouse( EMouseEvent eventType );

	// Mouse handling virtuals.
	virtual void mouseClicked( Mouse *mouse, Component *component );
	virtual void mouseReleased( Mouse *mouse, Component *component );
	virtual void mouseMoved( Mouse *mouse, Component *component );

	// Handling button input.
	virtual void buttonPressed( Button *button );
	virtual void buttonReleased( Button *button );

	// Interface handling.
	Dialog*	createDialog( const string& message );
	Alert*	createAlert( const string& message );
	Button*	createButton( const string& caption, float x = 0.0f, float y = 0.0f );
	void	showPopup(Popup* popup);
	void	hidePopup(Popup* popup);

private:
	// Application interfaces.
	Backpack*		backpack_;

	// User interface stacks.
	deque<Popup*>	popupStack_;
	vector<Popup*>	popupList_;
	vector<Button*>	buttonList_;

	// Input handling.
	Mouse*			mouse_;

	// Interface variables.
	Dialog			*loadDialog_;
	Alert			*alert_;
	Alert			*error_;
	Button			*deleteButton_, *craftButton_, *sortButton_;
};