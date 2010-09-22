#pragma once

#include "DirectX.h"
#include "Hashtable.h"
#include "KeyValueParser.h"
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
	stringAnyMap		*m_langDefinitions, *m_itemDefinitions;

	/* Application variables. */
	float		m_fCamera, m_fSpeed, m_fDestination;
};