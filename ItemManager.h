#pragma once

// Drawing API.
#include "DirectX.h"

// User interface headers.
#include "Button.h"
#include "Popup.h"
#include "Dialog.h"
#include "Alert.h"

// Item and definition headers.
#include "Hashtable.h"
#include "Inventory.h"
#include "SerializedBuffer.h"

#include <json/json.h>
#include <boost/regex.hpp>

#include <vector>
#include <sstream>
#include <string>

class ItemManager: public DirectX
{
public:
	ItemManager(HINSTANCE hInstance);
	~ItemManager();

	// Initializing and closing.
	void loadInterfaces();
	void closeInterfaces();

	// Item defining and loading.
	void loadDefinitions();
	void loadItems();

	// Drawing functions.
	void onFrame();
	void onRedraw();

	// Input handling.
	void onMouseDown();
	void onMouseUp();

	// Inventory handling.
	void handleCallbacks();

	// Interface handling.
	Dialog* createDialog(const string& newMsg);
	Alert* createAlert(const string& newMsg);
	Button* createButton(const string& newCaption, const float xNew = 0, const float yNew = 0);
	void	showPopup(Popup* whichPopup);
	void	hidePopup(Popup* whichPopup);

private:
	// Application interfaces.
	Inventory*	m_pInventory;

	// User interface stacks.
	vector<Popup*> m_popupStack;
	vector<Popup*> m_popupList;
	vector<Button*> m_buttonList;

	// Interface variables.
	Dialog		*m_loadDialog;
	Button		*m_deleteButton, *m_craftButton, *m_sortButton;

	/* Application variables. */
	float		m_fCamera, m_fSpeed, m_fDestination;
};