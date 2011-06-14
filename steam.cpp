#include "Steam.h"
#include "SerializedBuffer.h"

#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"

/* Function prototypes to load from DLL. */
bool* (*Steam_BGetCallback) (HSteamPipe hSteamPipe, CallbackMsg_t *pCallbackMsg, HSteamCall *phSteamCall);
void* (*Steam_FreeLastCallback) (HSteamPipe hSteamPipe);

Steam::Steam( void )
{
	// Set to null.
	steamClient_	= 0;
	hPipe_ = 0;
	hUser_ = 0;
}

Steam::~Steam( void )
{
	//Steam has been destroyed.
	CloseInterfaces();
}

void Steam::LoadInterfaces( void )
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
	if (!steamUser_->LoggedOn()) {
		throw Exception( "You are not properly logged into Steam." );
	}

	gameCoordinator_ = (ISteamGameCoordinator001*)steamClient_->GetISteamGenericInterface(
		hUser_, hPipe_,
		STEAMGAMECOORDINATOR_INTERFACE_VERSION_001 );

	if (!gameCoordinator_)
		throw Exception( "Failed to get ISteamGameCoordinator interface." );
}

void Steam::CloseInterfaces( void )
{
	if (steamClient_)
	{
		//Release user.
		if (hUser_) {
			steamClient_->ReleaseUser( hPipe_, hUser_ );
			hUser_ = 0;
		}

		//Release pipe.
		if (hPipe_) {
			steamClient_->ReleaseSteamPipe( hPipe_ );
			hPipe_ = 0;
		}
	}

	if (clientDll_) {
		FreeLibrary( clientDll_ );
		clientDll_ = 0;
	}

	// Finally, shut down API.
	SteamAPI_Shutdown();
}

bool Steam::GetCallback( CallbackMsg_t* tCallback )
{
	HSteamCall steamCall;
	return Steam_BGetCallback( hPipe_, tCallback, &steamCall );
}

void Steam::ReleaseCallback( void )
{
	Steam_FreeLastCallback( hPipe_ );
}

bool Steam::HasMessage( uint32* messageSize )
{
	return gameCoordinator_->IsMessageAvailable( messageSize );
}

void Steam::GetMessage( unsigned int* id, void* buffer, uint32 size, unsigned int* realSize )
{
	gameCoordinator_->RetrieveMessage( id, buffer, size, realSize );
}

void Steam::SendMessage( uint32 id, void* buffer, uint32 size )
{
	// Check if we need a protobuf header sent.
	if ((id & 0x80000000) != 0) {
		// Create header for response.
		CMsgProtoBufHeader responseHeader;
		GenerateProtobufHeader( &responseHeader );
		string headerData;
		responseHeader.SerializeToString( &headerData );

		// Fill in struct.
		GCProtobufHeader_t structHeader;
		structHeader.m_cubProtobufHeader = headerData.length();
		structHeader.m_EMsg = id;

		// Append messages.
		uint32 responseSize = sizeof( GCProtobufHeader_t ) + headerData.length() + size;
		void* response = malloc( responseSize );
		SerializedBuffer responseBuffer( response );
		responseBuffer.write( &structHeader, sizeof( GCProtobufHeader_t ) );
		responseBuffer.write( (void*)headerData.c_str(), headerData.length() );
		responseBuffer.write( buffer, size );
		gameCoordinator_->SendMessage( id, responseBuffer.start(), responseSize );
		free( response );
	}
	else {
		// Just send it.
		gameCoordinator_->SendMessage( id, buffer, size );
	}
}

uint64 Steam::GetSteamId( void ) const
{
	CSteamID steamId = steamUser_->GetSteamID();
	return steamId.ConvertToUint64();
}

void Steam::SetTargetId( uint64 targetId )
{
	targetId_ = targetId;
}

uint64 Steam::GetTargetId( void ) const
{
	return targetId_;
}

void Steam::GenerateProtobufHeader( CMsgProtoBufHeader *headerMsg ) const
{
	headerMsg->set_client_steam_id( GetSteamId() );
	headerMsg->set_job_id_target( GetTargetId() );
}