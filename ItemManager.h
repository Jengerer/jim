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
#include "Inventory.h"
#include "SerializedBuffer.h"

class ItemManager: public DirectX, public ButtonListener
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
	void onButtonClick( Button* button );
	void onButtonRelease( Button* button );

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
	deque<Popup*>	popupStack_;
	vector<Popup*>	popupList_;
	vector<Button*>	buttonList_;

	// Interface variables.
	Dialog			*loadDialog_;
	Alert			*alert_;
	Button			*deleteButton_, *craftButton_, *sortButton_;
};