#pragma once

#include "DirectX.h"
#include "Hashtable.h"
#include "KeyValueParser.h"
#include "Inventory.h"

#include <json/json.h>
#include <boost/regex.hpp>

#include <vector>
#include <sstream>
#include <string>

// Window properties.
#define APPLICATION_TITLE		"Jengerer's Item Manager"
#define APPLICATION_WIDTH		800
#define APPLICATION_HEIGHT		500

// Application attributes.
#define APPLICATION_FRAMERATE	30.0f
#define APPLICATION_FRAMESPEED	1000.0f/APPLICATION_FRAMERATE
#define APPLICATION_VERSION		1000

using namespace std;

class ItemManager: public DirectX
{
public:
	ItemManager(HINSTANCE hInstance);
	~ItemManager();

	/* Initializing and closing. */
	void loadInterfaces();
	void closeInterfaces();

	/* Drawing functions. */
	void onFrame();
	void onRedraw();

	/* Input handling. */
	void onMouseDown();
	void onMouseUp();

	// Inventory handling.
	void handleCallbacks();

	// Locking and loading inventory.
	void setLocked(bool isLocked);
	void setLoaded(bool isLoaded);
	bool isLoaded() const;
	bool isLocked() const;

	/* Item defining. */
	void loadDefinitions();
	void loadItems();

private:
	/* Application interfaces. */
	Inventory*	m_pInventory;

	/* Inventory variables. */
	bool		m_bLoaded; //Items loaded?
	bool		m_bLocked; //Read-only permissions?

	// Item definitions.
	stringHashMap		*m_langDefinitions, *m_itemDefinitions;

	/* Application variables. */
	float		m_fCamera, m_fSpeed, m_fDestination;
};