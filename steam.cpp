#include "steam.hpp"
#include "serialized_buffer.hpp"
#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"
#include <string/string.hpp>
#include <jui/application/error_stack.hpp>

/* Function prototypes to load from DLL. */
bool (*Steam_BGetCallback) (HSteamPipe hSteamPipe, CallbackMsg_t *pCallbackMsg, HSteamCall *phSteamCall);
void (*Steam_FreeLastCallback) (HSteamPipe hSteamPipe);

Steam::Steam( void )
{
	// Set to null.
	steam_client_	= nullptr;
	pipe_			= 0;
	user_			= 0;
}

Steam::~Steam( void )
{
	//Steam has been destroyed.
	close_interfaces();
}

/*
 * Load Steam interfaces.
 */
bool Steam::load_interfaces( void )
{
    // Get error stack for reporting.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Set Team Fortress 2 application ID.
	SetEnvironmentVariable( "SteamAppId", "440" );
    JUTIL::ArrayBuilder<char> install_path;
	
	// Initial allocation.
	const size_t DEFAULT_PATH_SIZE = 32;
	if (!install_path.set_size( DEFAULT_PATH_SIZE )) {
		stack->log( "Failed to allocate initial string for Steam install path registry value." );
		return false;
	}

	// Get steam path.
	HKEY key;
	bool reg_success = false;
	if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Valve\\Steam\\", 0, KEY_QUERY_VALUE, &key ) == ERROR_SUCCESS) {
		DWORD reg_type;
		DWORD reg_size = DEFAULT_PATH_SIZE;

		// Resize if necessary.
		LSTATUS status = RegQueryValueEx( key, "InstallPath", nullptr, &reg_type, (LPBYTE)install_path.get_array(), &reg_size );
		if (status == ERROR_MORE_DATA) {
            if (!install_path.set_size( reg_size )) {
                stack->log( "Failed to allocate buffer for Steam install path." );
                return false;
            }

			// Now read into buffer.
			status = RegQueryValueEx( key, "InstallPath", nullptr, &reg_type, (LPBYTE)install_path.get_array(), &reg_size );
		}

		// Set directory to load Steam client DLL from.
		if (status == ERROR_SUCCESS) {
			SetDllDirectory( install_path.get_array() );
			reg_success = true;
		}
	}

	// Free buffer.
	install_path.clear();
	RegCloseKey( key );
	if (!reg_success) {
        stack->log( "Failed to get Steam install path from registry." );
        return false;
	}

	// Load the library.
	client_dll_ = LoadLibrary( "steamclient.dll" );
	if (client_dll_ == nullptr) {
        stack->log( "Failed to load steamclient.dll from Steam install path." );
        return false;
	}

	// Now define the functions.
	Steam_BGetCallback = (bool (*)(HSteamPipe, CallbackMsg_t *, HSteamCall *))GetProcAddress( client_dll_, "Steam_BGetCallback" );
	if (Steam_BGetCallback == nullptr) {
        stack->log( "Failed to get Steam_BGetCallback from Steam library." );
        return false;
	}
	Steam_FreeLastCallback = (void (*)(HSteamPipe))GetProcAddress( client_dll_, "Steam_FreeLastCallback" );
	if (Steam_FreeLastCallback == nullptr) {
        stack->log( "Failed to get Steam_FreeLastCallback from Steam library." );
        return false;
	}

	//Now load the API.
	CSteamAPILoader apiLoader;
	CreateInterfaceFn clientFactory = apiLoader.Load();
	if (clientFactory == nullptr) {
		stack->log( "Failed to create Steam client factory from API." );
        return false;
	}

	steam_client_ = (ISteamClient008*)clientFactory( STEAMCLIENT_INTERFACE_VERSION_008, 0 );
	if (steam_client_ == nullptr) {
        stack->log( "Failed to create Steam client interface." );
        return false;
	}

	// Down to to the nitty-gritty. Start 'er up!
	if (!SteamAPI_Init()) {
		stack->log( "Failed to initialize Steam API. Make sure Steam is running and try again." );
        return false;
	}

	pipe_ = steam_client_->CreateSteamPipe();
	user_ = steam_client_->ConnectToGlobalUser( pipe_ );

	// Make sure we've got a user.
	if (user_ == 0) {
        stack->log( "Failed to connect to global user." );
        return false;
	}

	// Make sure we're logged on correctly.
	steam_user_ = (ISteamUser012*)steam_client_->GetISteamUser(
		user_,
		pipe_,
		STEAMUSER_INTERFACE_VERSION_012 );
	if (steam_user_ == nullptr || !steam_user_->LoggedOn()) {
		stack->log( "You are not properly logged into Steam." );
        return false;
	}

    // Create game coordinator interface.
	game_coordinator_ = (ISteamGameCoordinator001*)steam_client_->GetISteamGenericInterface(
		user_, pipe_,
		STEAMGAMECOORDINATOR_INTERFACE_VERSION_001 );
	if (game_coordinator_ == nullptr) {
		stack->log( "Failed to get ISteamGameCoordinator interface." );
        return false;
	}
    return true;
}

/*
 * Clean up Steam interfaces.
 */
void Steam::close_interfaces( void )
{
    // Release user and pipe.
	if (steam_client_ != nullptr) {
        // Release user.
		if (user_ != 0) {
			steam_client_->ReleaseUser( pipe_, user_ );
			user_ = 0;
		}

        // Release pipe.
		if (pipe_ != 0) {
			steam_client_->ReleaseSteamPipe( pipe_ );
			pipe_ = 0;
		}
	}

    // Release DLL handle.
	if (client_dll_ != nullptr) {
		FreeLibrary( client_dll_ );
		client_dll_ = nullptr;
	}

	// Finally, shut down API.
	SteamAPI_Shutdown();
}

/*
 * Get callback waiting on Steam.
 */
bool Steam::get_callback( CallbackMsg_t* tCallback ) const
{
	HSteamCall steamCall;
	return Steam_BGetCallback( pipe_, tCallback, &steamCall );
}

/*
 * Release previous callback.
 */
void Steam::release_callback( void ) const
{
	Steam_FreeLastCallback( pipe_ );
}

/*
 * Check if message exists, and if it does, get message size.
 */
bool Steam::has_message( uint32* size ) const
{
	return game_coordinator_->IsMessageAvailable( size );
}

/*
 * Get message from coordinator.
 */
bool Steam::get_message( unsigned int* id, void* buffer, uint32 size, unsigned int* real_size ) const
{
    // Get error stack for reporting.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();
	EGCResults result = game_coordinator_->RetrieveMessage( id, buffer, size, real_size );
    if (result != k_EGCResultOK) {
        stack->log( "Failed to retrieve message from game coordinator." );
        return false;
    }
    return true;
}

/*
 * Send a message through coordinator.
 */
bool Steam::send_message( uint32 id, void* buffer, uint32 size ) const
{
	// Check if we need a protobuf header sent.
    EGCResults result;
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
		uint32 response_size = sizeof( GCProtobufHeader_t ) + headerData.length() + size;
        JUTIL::ArrayBuilder<char> response;
        if (!response.set_size( response_size )) {
            return false;
        }
		SerializedBuffer response_buffer( response.get_array() );
		response_buffer.write( &structHeader, sizeof( GCProtobufHeader_t ) );
		response_buffer.write( (void*)headerData.c_str(), headerData.length() );
		response_buffer.write( buffer, size );
		result = game_coordinator_->SendMessage( id, response_buffer.start(), response_size );
	}
	else {
		// Just send it.
		result = game_coordinator_->SendMessage( id, buffer, size );
	}

    // Check if sent successfully.
    if (result != k_EGCResultOK) {
        return false;
    }
    return true;
}

/*
 * Set Steam inventory/schema version.
 */
void Steam::set_version( uint64 version )
{
	version_ = version;
}

/*
 * Get Steam inventory/schema version.
 */
uint64 Steam::get_version( void ) const
{
	return version_;
}

/*
 * Get user Steam ID as 64-bit integer.
 */
uint64 Steam::get_steam_id( void ) const
{
	CSteamID steamId = steam_user_->GetSteamID();
	return steamId.ConvertToUint64();
}

/*
 * Set target of protobuf messages.
 */
void Steam::set_target_id( uint64 targetId )
{
	target_id_ = targetId;
}

/*
 * Get target of protobuf messages.
 */
uint64 Steam::get_target_id( void ) const
{
	return target_id_;
}

/*
 * Fill out protobuf header message.
 */
void Steam::generate_protobuf_header( CMsgProtoBufHeader *header ) const
{
	header->set_client_steam_id( get_steam_id() );
	header->set_job_id_target( get_target_id() );
}
