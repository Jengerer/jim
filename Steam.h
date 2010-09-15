#pragma once

#include "steam/SteamclientAPI.h"
#include "Item.h"
#include "Exception.h"

#include <vector>

using namespace std;

class Steam
{
public:
	Steam();
	~Steam();

	/* Initializing and closing. */
	void loadInterfaces();
	void closeInterfaces();

	/* Callback and message handling. */
	bool getCallback(CallbackMsg_t* tCallback);
	void releaseCallback();
	bool hasMessage(uint32* messageSize);
	void getMessage(unsigned int* messageID, void* messageBuffer, uint32 iSize, unsigned int* iReal);

	/* Interface handling. */
	void updateItem(Item* whichItem);
	void deleteItem(uint64 itemID);

	/* Steam getters. */
	uint64 getSteamID() const;

private:
	/* Steam handlers. */
	HSteamUser					m_hUser;
	HSteamPipe					m_hPipe;

	/* Steam interfaces. */
	ISteamClient008*			m_pClient;
	ISteamUser012*				m_pUser;
	ISteamGameCoordinator001*	m_pCoordinator;
};