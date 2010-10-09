#include "Steam.h"

/* Function prototypes to load from DLL. */
bool* (*Steam_BGetCallback) (HSteamPipe hSteamPipe, CallbackMsg_t *pCallbackMsg, HSteamCall *phSteamCall);
void* (*Steam_FreeLastCallback) (HSteamPipe hSteamPipe);

Steam::Steam()
{
	// Set to null.
	steamClient_	= 0;
}

Steam::~Steam()
{
	//Steam has been destroyed.
	closeInterfaces();
}

void Steam::openInterfaces()
{

	// Set Team Fortress 2 application ID.
	SetEnvironmentVariable( "SteamAppId", "440" );

	// Get Steam directory.
	char steamPath[512];
	HKEY hkRegistry;

	if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Valve\\Steam\\", 0, KEY_QUERY_VALUE, &hkRegistry ) == ERROR_SUCCESS)
	{
		// Clear the string.
		memset( steamPath, 0, 512 );

		DWORD regType, regSize=511;
		if (RegQueryValueEx( hkRegistry, "InstallPath", 0, &regType, (unsigned char *)steamPath, &regSize ) == ERROR_SUCCESS)
			SetDllDirectory( steamPath );
		else
			throw Exception( "Failed to get Steam install path from registry." );
	} else
	{
		throw Exception( "Failed to get Steam install path from registry." );
	}

	//Load the library.
	clientDll_ = LoadLibrary( "steamclient.dll" );
	if ( !clientDll_ )
		throw Exception( "Failed to load steamclient.dll from Steam install path." );

	//Now define the functions.
	if (!Steam_BGetCallback)
		Steam_BGetCallback = (bool* (*)(HSteamPipe, CallbackMsg_t *, HSteamCall *))GetProcAddress( clientDll_, "Steam_BGetCallback" );

	if (!Steam_FreeLastCallback)
		Steam_FreeLastCallback = (void* (*)(HSteamPipe))GetProcAddress( clientDll_, "Steam_FreeLastCallback" );

	//Now load the API.
	CSteamAPILoader apiLoader;

	CreateInterfaceFn clientFactory = apiLoader.Load();
	if (clientFactory == NULL)
		throw Exception( "Failed to load Steam API from factory." );

	steamClient_ = (ISteamClient008*)clientFactory( STEAMCLIENT_INTERFACE_VERSION_008, 0 );
	if (!steamClient_)
		throw Exception( "Failed to create Steam client interface." );

	//Down to to the nitty-gritty. Start 'er up!
	if (!SteamAPI_Init())
		throw Exception( "Failed to initialize Steam API. Make sure Steam is running and try again." );

	hPipe_ = steamClient_->CreateSteamPipe();
	hUser_ = steamClient_->ConnectToGlobalUser(hPipe_);

	// Make sure we've got a user.
	if (hUser_ == 0)
		throw Exception( "Failed to connect to global user." );

	// Make sure we're logged on correctly.
	steamUser_ = (ISteamUser012*)steamClient_->GetISteamUser(
		hUser_,
		hPipe_,
		STEAMUSER_INTERFACE_VERSION_012 );
	if (!steamUser_->LoggedOn())
		throw Exception( "You're not properly logged into Steam." );

	gameCoordinator_ = (ISteamGameCoordinator001*)steamClient_->GetISteamGenericInterface(
		hUser_, hPipe_,
		STEAMGAMECOORDINATOR_INTERFACE_VERSION_001 );

	// Get friends.
	steamFriends_ = (ISteamFriends005*)steamClient_->GetISteamFriends(
		hUser_, hPipe_,
		STEAMFRIENDS_INTERFACE_VERSION_005 );

	if (!gameCoordinator_)
		throw Exception( "Failed to get ISteamGameCoordinator interface." );
}

void Steam::closeInterfaces()
{
	if (steamClient_)
	{
		//Release user.
		if (hUser_)
			steamClient_->ReleaseUser( hPipe_, hUser_ );

		//Release pipe.
		if (hPipe_)
			steamClient_->ReleaseSteamPipe( hPipe_ );
	}

	if (clientDll_)
		FreeLibrary( clientDll_ );

	//Finally, shut down API.
	SteamAPI_Shutdown();
}

bool Steam::getCallback( CallbackMsg_t* tCallback )
{
	HSteamCall hSteamCall;
	if (Steam_BGetCallback( hPipe_, tCallback, &hSteamCall ))
		return true;
	else
		return false;
}

void Steam::releaseCallback()
{
	Steam_FreeLastCallback( hPipe_ );
}

bool Steam::hasMessage( uint32* messageSize )
{
	return gameCoordinator_->IsMessageAvailable(messageSize);
}

void Steam::getMessage( unsigned int* id, void* buffer, uint32 size, unsigned int* realSize )
{
	gameCoordinator_->RetrieveMessage( id, buffer, size, realSize );
}

void Steam::deleteItem( uint64 itemId )
{
	// Generate message.
	SOMsgDeleted_t message;
	memset( &message, 0xFF, sizeof( SOMsgUpdate_t ) );

	message.itemid = itemId;
	
	// Send it.
	sendMessage( 
		SOMsgDeleted_t::k_iMessage,
		&message,
		sizeof( SOMsgDeleted_t ) );
}

void Steam::sendMessage( uint32 id, void* message, uint32 size )
{
	gameCoordinator_->SendMessage( id, message, size );
}

uint64 Steam::getSteamId() const
{
	CSteamID steamId = steamUser_->GetSteamID();
	return steamId.ConvertToUint64();
}

int Steam::getFriends()
{
	return steamFriends_->GetFriendCount( k_EFriendFlagAll );
}

CSteamID Steam::getFriend( int index )
{
	return steamFriends_->GetFriendByIndex( index, k_EFriendFlagAll );
}

bool Steam::getFriendGameInfo( CSteamID steamId, FriendGameInfo_t* friendGameInfo )
{
	return steamFriends_->GetFriendGamePlayed( steamId, friendGameInfo );
}

const char* Steam::getPersonaName( CSteamID steamId )
{
	return steamFriends_->GetFriendPersonaName( steamId );
}