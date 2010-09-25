#pragma once

#include <vector>
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

// Items and definitions.
#include "Hashtable.h"
#include "Inventory.h"
#include "SerializedBuffer.h"

class ItemManager: public DirectX
{
public:
	ItemManager( HINSTANCE hInstance );
	~ItemManager();

	// Initializing and closing.
	void openInterfaces();
	void closeInterfaces();

	// Item defining and loading.
	void loadDefinitions();
	void loadItems();

	// Drawing functions.
	void onFrame();
	void onRedraw();

	// Input handling.
	void onMouseClick();
	void onMouseRelease();
	void onMouseMove();

	// Inventory handling.
	void handleCallbacks();

	// Interface handling.
	Dialog*	createDialog( const string& message );
	Alert*	createAlert( const string& message );
	Button*	createButton( const string& caption, float x = 0.0f, float y = 0.0f );
	void	showPopup(Popup* popup);
	void	hidePopup(Popup* popup);

private:
	// Application interfaces.
	Inventory*		inventory_;

	// User interface stacks.
	vector<Popup*>	popupStack;
	vector<Popup*>	popups;
	vector<Button*>	buttons;

	// Interface variables.
	Dialog			*loadDialog_;
	Button			*deleteButton_, *craftButton_, *sortButton_;
};