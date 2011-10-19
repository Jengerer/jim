#include "steam.h"
#include "serialized_buffer.h"

#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"

#ifdef _DEBUG
#include <fstream>
#endif

/* Function prototypes to load from DLL. */
bool (*Steam_BGetCallback) (HSteamPipe hSteamPipe, CallbackMsg_t *pCallbackMsg, HSteamCall *phSteamCall);
void (*Steam_FreeLastCallback) (HSteamPipe hSteamPipe);

Steam::Steam( void )
{
	// Set to null.
	steamClient_	= nullptr;
	hPipe_			= 0;
	hUser_			= 0;
}

Steam::~Steam( void )
{
	//Steam has been destroyed.
	close_interfaces();
}

void Steam::load_interfaces( void )
{
	// Set Team Fortress 2 application ID.
	SetEnvironmentVariable( "SteamAppId", "440" );
	char* steamPath = (char*)malloc(1);

	// Get steam path.
	HKEY hKey;
	bool regSuccess = false;
	if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Valve\\Steam\\", 0, KEY_QUERY_VALUE, &hKey ) == ERROR_SUCCESS) {
		DWORD regType;
		DWORD regSize = 1;

		// First make proper buffer size.
		LSTATUS status;
		if ((status = RegQueryValueEx( hKey, "InstallPath", nullptr, &regType, (LPBYTE)steamPath, &regSize )) == ERROR_MORE_DATA) {
			steamPath = (char*)realloc( steamPath, regSize );
		}

		status = RegQueryValueEx( hKey, "InstallPath", nullptr, &regType, (LPBYTE)steamPath, &regSize );
		if (status == ERROR_SUCCESS) {
			SetDllDirectory( steamPath );
			regSuccess = true;
		}
	}

	// Free buffer.
	free( steamPath );
	steamPath = nullptr;

	RegCloseKey( hKey );
	if (!regSuccess) {
		throw std::runtime_error( "Failed to get Steam install path from registry." );
	}

	//Load the library.
	clientDll_ = LoadLibrary( "steamclient.dll" );
	if ( clientDll_ == nullptr ) {
		throw std::runtime_error( "Failed to load steamclient.dll from Steam install path." );
	}

	//Now define the functions.
	Steam_BGetCallback = (bool (*)(HSteamPipe, CallbackMsg_t *, HSteamCall *))GetProcAddress( clientDll_, "Steam_BGetCallback" );
	if (Steam_BGetCallback == nullptr) {
		throw std::runtime_error( "Failed to get Steam_BGetCallback from Steam library." );
	}

	Steam_FreeLastCallback = (void (*)(HSteamPipe))GetProcAddress( clientDll_, "Steam_FreeLastCallback" );
	if (Steam_FreeLastCallback == nullptr) {
		throw std::runtime_error( "Failed to get Steam_FreeLastCallback from Steam library." );
	}

	//Now load the API.
	CSteamAPILoader apiLoader;

	CreateInterfaceFn clientFactory = apiLoader.Load();
	if (clientFactory == nullptr) {
		throw std::runtime_error( "Failed to load Steam API from factory." );
	}

	steamClient_ = (ISteamClient008*)clientFactory( STEAMCLIENT_INTERFACE_VERSION_008, 0 );
	if (steamClient_ == nullptr) {
		throw std::runtime_error( "Failed to create Steam client interface." );
	}

	// Down to to the nitty-gritty. Start 'er up!
	if (!SteamAPI_Init()) {
		throw std::runtime_error( "Failed to initialize Steam API. Make sure Steam is running and try again." );
	}

	hPipe_ = steamClient_->CreateSteamPipe();
	hUser_ = steamClient_->ConnectToGlobalUser( hPipe_ );

	// Make sure we've got a user.
	if (hUser_ == 0) {
		throw std::runtime_error( "Failed to connect to global user." );
	}

	// Make sure we're logged on correctly.
	steamUser_ = (ISteamUser012*)steamClient_->GetISteamUser(
		hUser_,
		hPipe_,
		STEAMUSER_INTERFACE_VERSION_012 );
	if (steamUser_ == nullptr || !steamUser_->LoggedOn()) {
		throw std::runtime_error( "You are not properly logged into Steam." );
	}

	gameCoordinator_ = (ISteamGameCoordinator001*)steamClient_->GetISteamGenericInterface(
		hUser_, hPipe_,
		STEAMGAMECOORDINATOR_INTERFACE_VERSION_001 );
	if (gameCoordinator_ == nullptr) {
		throw std::runtime_error( "Failed to get ISteamGameCoordinator interface." );
	}
}

void Steam::close_interfaces( void )
{
	if (steamClient_ != nullptr) {
		if (hUser_ != 0) {
			steamClient_->ReleaseUser( hPipe_, hUser_ );
			hUser_ = 0;
		}

		if (hPipe_ != 0) {
			steamClient_->ReleaseSteamPipe( hPipe_ );
			hPipe_ = 0;
		}
	}

	if (clientDll_ != nullptr) {
		FreeLibrary( clientDll_ );
		clientDll_ = nullptr;
	}

	// Finally, shut down API.
	SteamAPI_Shutdown();
}

bool Steam::get_callback( CallbackMsg_t* tCallback ) const
{
	HSteamCall steamCall;
	return Steam_BGetCallback( hPipe_, tCallback, &steamCall );
}

void Steam::release_callback( void ) const
{
	Steam_FreeLastCallback( hPipe_ );
}

bool Steam::has_message( uint32* messageSize ) const
{
	return gameCoordinator_->IsMessageAvailable( messageSize );
}

void Steam::get_message( unsigned int* id, void* buffer, uint32 size, unsigned int* realSize ) const
{
	gameCoordinator_->RetrieveMessage( id, buffer, size, realSize );
}

void Steam::send_message( uint32 id, void* buffer, uint32 size ) const
{
//#ifdef _DEBUG
	std::ofstream log;
	log.open( "message_log.txt", std::ios::app );
	log << "Sent message of type " << id << ".\n";
	log.close();
//#endif

	// Check if we need a protobuf header sent.
	if ((id & 0x80000000) != 0) {
		// Create header for response.
		CMsgProtoBufHeader responseHeader;
		generate_protobuf_header( &responseHeader );
		std::string headerData;
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

void Steam::set_version( uint64 version )
{
	version_ = version;
}

uint64 Steam::get_version() const
{
	return version_;
}

uint64 Steam::get_steam_id( void ) const
{
	CSteamID steamId = steamUser_->GetSteamID();
	return steamId.ConvertToUint64();
}

void Steam::set_target_id( uint64 targetId )
{
	targetId_ = targetId;
}

uint64 Steam::get_target_id( void ) const
{
	return targetId_;
}

void Steam::generate_protobuf_header( CMsgProtoBufHeader *headerMsg ) const
{
	headerMsg->set_client_steam_id( get_steam_id() );
	headerMsg->set_job_id_target( get_target_id() );
}