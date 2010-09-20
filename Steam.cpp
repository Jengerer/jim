#include "Steam.h"

/* Function prototypes to load from DLL. */
bool* (*Steam_BGetCallback) (HSteamPipe hSteamPipe, CallbackMsg_t *pCallbackMsg, HSteamCall *phSteamCall);
void* (*Steam_FreeLastCallback) (HSteamPipe hSteamPipe);

Steam::Steam()
{
	// Steam has been created.
	try {
		loadInterfaces();
	} catch (Exception steamException) {
		// Shut down Steam, and pass exception.
		closeInterfaces();
		throw steamException;
	}
}

Steam::~Steam()
{
	//Steam has been destroyed.
	closeInterfaces();
}

void Steam::loadInterfaces()
{
	//We're loading TF2.
	SetEnvironmentVariable("SteamAppId", "440");

	//Get Steam directory.
	char SteamPath[512];
	HKEY hkRegistry;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Valve\\Steam\\", 0, KEY_QUERY_VALUE, &hkRegistry) == ERROR_SUCCESS)
	{
		//Clear the string.
		memset(SteamPath, 0, 512);

		DWORD regType, regSize=511;
		if (RegQueryValueEx(hkRegistry, "InstallPath", 0, &regType, (unsigned char *)SteamPath, &regSize) == ERROR_SUCCESS)
			SetDllDirectory(SteamPath);
		else
			throw Exception("Failed to get Steam install path from registry.");
	} else
	{
		throw Exception("Failed to get Steam install path from registry.");
	}

	//Load the library.
	HMODULE ClientDLL = LoadLibrary("steamclient.dll");
	if (ClientDLL == NULL)
		throw Exception("Failed to load steamclient.dll from Steam install path.");

	//Now define the functions.
	if (!Steam_BGetCallback)
		Steam_BGetCallback = (bool* (*)(HSteamPipe, CallbackMsg_t *, HSteamCall *))GetProcAddress(ClientDLL, "Steam_BGetCallback");

	if (!Steam_FreeLastCallback)
		Steam_FreeLastCallback = (void* (*)(HSteamPipe))GetProcAddress(ClientDLL, "Steam_FreeLastCallback");

	//Now load the API.
	CSteamAPILoader apiLoader;

	CreateInterfaceFn clientFactory = apiLoader.Load();
	if (clientFactory == NULL)
		throw Exception("Failed to load Steam API from factory.");

	m_pClient = (ISteamClient008*)clientFactory(STEAMCLIENT_INTERFACE_VERSION_008, NULL);
	if (m_pClient == NULL)
		throw Exception("Failed to create Steam client interface.");

	//Down to to the nitty-gritty. Start 'er up!
	if (!SteamAPI_Init())
		throw Exception("Failed to initialize Steam API. Make sure Steam is running and try again.");

	m_hPipe = m_pClient->CreateSteamPipe();
	m_hUser = m_pClient->ConnectToGlobalUser(m_hPipe);

	//Make sure we've got a user.
	if (m_hUser == 0)
		throw Exception("Failed to connect to global user.");

	//Make sure we're logged on correctly.
	m_pUser = (ISteamUser012*)m_pClient->GetISteamUser(m_hUser, m_hPipe, STEAMUSER_INTERFACE_VERSION_012);
	if (!m_pUser->LoggedOn())
		throw Exception("You're not properly logged into Steam.");

	m_pCoordinator = (ISteamGameCoordinator001*)m_pClient->GetISteamGenericInterface(
		m_hUser, m_hPipe,
		STEAMGAMECOORDINATOR_INTERFACE_VERSION_001);

	if (!m_pCoordinator)
		throw Exception("Failed to get ISteamGameCoordinator interface.");

	ISteamFriends001* steamFriends = (ISteamFriends001*)m_pClient->GetISteamFriends(m_hUser, m_hPipe, STEAMFRIENDS_INTERFACE_VERSION_001);
	steamFriends->SetPersonaState(k_EPersonaStateOnline);
}

void Steam::closeInterfaces()
{
	//Release user.
	if (m_hUser)
		m_pClient->ReleaseUser(m_hPipe, m_hUser);

	//Release pipe.
	if (m_hPipe)
		m_pClient->ReleaseSteamPipe(m_hPipe);

	//Finally, shut down API.
	SteamAPI_Shutdown();
}

bool Steam::getCallback(CallbackMsg_t* tCallback)
{
	HSteamCall hSteamCall;
	if (Steam_BGetCallback(m_hPipe, tCallback, &hSteamCall))
		return true;
	else
		return false;
}

void Steam::releaseCallback()
{
	Steam_FreeLastCallback(m_hPipe);
}

bool Steam::hasMessage(uint32* messageSize)
{
	return m_pCoordinator->IsMessageAvailable(messageSize);
}

void Steam::getMessage(unsigned int* messageID, void* messageBuffer, uint32 iSize, unsigned int* sizeReal)
{
	m_pCoordinator->RetrieveMessage(messageID, messageBuffer, iSize, sizeReal);
}

void Steam::updateItem(Item* whichItem)
{
	/* Generate the message with the new flags. */
	SOMsgUpdate_t msgUpdate;
	msgUpdate.itemID = whichItem->getUniqueID();
	msgUpdate.position = whichItem->getFlags();

	/* Send it. */
	m_pCoordinator->SendMessage(SOMsgUpdate_t::k_iMessage, &msgUpdate, sizeof(msgUpdate));
}

void Steam::deleteItem(uint64 itemID)
{
	/* Generate the message. */
	SOMsgDeleted_t msgDelete;
	msgDelete.itemid = itemID;
	
	/* Send it. */
	m_pCoordinator->SendMessage(SOMsgDeleted_t::k_iMessage, &msgDelete, sizeof(msgDelete));
}

uint64 Steam::getSteamID() const
{
	CSteamID steamID = m_pUser->GetSteamID();
	return steamID.ConvertToUint64();
}