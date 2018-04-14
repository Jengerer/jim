#include "updater.hpp"
#include <windows.h>
#include <json/json.h>
#include <jui/application/error_stack.hpp>
#include <jui/jui_version.hpp>
#include <jutil_version.hpp>

// Version checking parameters.
const JUTIL::ConstantString VERSIONS_URL( "http://www.jengerer.com/item_manager/item_manager.json" );

// File removal parameters.
const long REMOVE_WAIT_INTERVAL = 250;
const unsigned int REMOVE_RETRY_MAXIMUM = 8;

// List of files to remove/update.
const unsigned int UPDATE_FILES_COUNT = 3;
const JUTIL::ConstantString JIM_FILE( "item_manager.exe" );
const JUTIL::ConstantString JUTIL_FILE( "jutil.dll" );
const JUTIL::ConstantString JUI_FILE( "jui.dll" );
const JUTIL::ConstantString* UPDATE_FILES[UPDATE_FILES_COUNT] = {
	&JIM_FILE,
	&JUTIL_FILE,
	&JUI_FILE
};

/*
 * Just set the downloader object we're using to get the files.
 */
Updater::Updater( JUI::FileDownloader* downloader, HttpResourceLoader* site_loader,	const JUTIL::String* jim_version )
	: downloader_( downloader ),
	  site_loader_( site_loader ),
	  jim_version_( jim_version ),
	  status_( UPDATE_PENDING )
{
}

/*
 * Clean up.
 */
Updater::~Updater( void )
{
}

/*
 * Clean up the old version of the files.
 */
bool Updater::remove_old_files( void )
{
	// Stack for reporting errors.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Look for old files.
	unsigned int i;
	for (i = 0; i < UPDATE_FILES_COUNT; ++i) {
		JUTIL::DynamicString old_filename;
		if (!old_filename.write( "%s.old", UPDATE_FILES[i]->get_string() )) {
			stack->log( "Failed to write old file name to buffer." );
			return false;
		}

		// Keep trying to delete the file if it exists.
		FILE* file;
		errno_t error = fopen_s( &file, old_filename.get_string(), "r" );
		if (file != nullptr) {
			fclose( file );

			// Try to delete the file now.
			unsigned int j;
			bool removed_file = false;
			for (j = 0; j < REMOVE_RETRY_MAXIMUM; ++j) {
				if (DeleteFile( old_filename.get_string() )) {
					removed_file = true;
					break;
				}
				Sleep( REMOVE_WAIT_INTERVAL );
			}

			// Did we get rid of it?
			if (!removed_file) {
				stack->log( "Took too long to get rid of %s.", old_filename.get_string() );
				return false;
			}
		}
	}

	return true;
}

/*
 * Get version numbers and update component state.
 */
UpdateCheckResult Updater::check_version_numbers( void )
{
	// Stack for error reporting.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Read latest version information.
	JUTIL::DynamicString versions;
	if (!downloader_->read( &VERSIONS_URL, &versions )) {
		// If server goes down, don't bother complaining.
		return UP_TO_DATE;
	}

	// Parse the JSON.
	Json::Value root;
	Json::CharReaderBuilder builder;
	Json::CharReader* reader = builder.newCharReader();
	const char* versions_start = versions.get_string();
	const char* versions_end = versions_start + versions.get_length();
	if (!reader->parse( versions_start, versions_end, &root, NULL )) {
		stack->log( "Failed to parse version file." );
		return CHECK_FAILED;
	}

	// Get version numbers.
	const JUTIL::ConstantString JUTIL_VERSION_KEY = "jutil_version";
	const JUTIL::ConstantString JUI_VERSION_KEY = "jui_version";
	const JUTIL::ConstantString JIM_VERSION_KEY = "jim_version";
	Json::Value& jim_version = root[ JIM_VERSION_KEY.get_string() ];
	Json::Value& jui_version = root[ JUI_VERSION_KEY.get_string() ];
	Json::Value& jutil_version = root[ JUTIL_VERSION_KEY.get_string() ];
	if (jutil_version.isNull() || jui_version.isNull() || jim_version.isNull()) {
		stack->log( "Bad format for latest versions." );
		return CHECK_FAILED;
	}
	const JUTIL::ConstantString JUTIL_LATEST( jutil_version.asCString() );
	const JUTIL::ConstantString JUI_LATEST( jui_version.asCString() );
	const JUTIL::ConstantString JIM_LATEST( jim_version.asCString() );

	// If any of the components are out of date, return that we need to update.
	const JUTIL::String* JUTIL_CURRENT = JUTIL::get_version_string();
	const JUTIL::String* JUI_CURRENT = JUI::get_version_string();
	is_jutil_latest_ = JUTIL_LATEST.is_equal( JUTIL_CURRENT );
	is_jui_latest_ = JUI_LATEST.is_equal( JUI_CURRENT );
	is_jim_latest_ = JIM_LATEST.is_equal( jim_version_ );
	if (!(is_jutil_latest_ && is_jui_latest_ && is_jim_latest_)) {
		return UPDATE_REQUIRED;
	}
	return UP_TO_DATE;
}

/*
 * Update all files that are out of date.
 */
bool Updater::update( void )
{
	// Compare version and download/move for each file/version.
	if (!is_jutil_latest_ && !update_file( &JUTIL_FILE )) {
		return false;
	}
	else if (!is_jui_latest_ && !update_file( &JUI_FILE )) {
		return false;
	}
	else if (!is_jim_latest_ && !update_file( &JIM_FILE )) {
		return false;
	}
	status_ = UPDATE_SUCCESSFUL;
	return true;
}

/*
 * Run the new version of the item manager.
 */
bool Updater::run_new_version( void )
{
	// Error stack for logging.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Create structs.
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	// Create copy of file name.
	JUTIL::DynamicString file;
	if (!file.copy( &JIM_FILE )) {
		return false;
	}
	if (!CreateProcess( NULL, file.get_string(), NULL, NULL, FALSE, 0, NULL, NULL, &si,	&pi)) {
		return false;
	}
	return true;
}

/*
 * Get update status.
 */
UpdateStatus Updater::get_update_status( void )
{
	return status_;
}

/*
 * Compare version and perform update on file if out of date.
 */
bool Updater::update_file( const JUTIL::String* file )
{
	// Stack for reporting errors.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Get name for new file.
	JUTIL::DynamicString new_name;
	if (!new_name.write( "%s.new", file->get_string() )) {
		status_ = UPDATE_FAILED;
		return false;
	}

	// Save the new version before moving the old one.
	if (!site_loader_->update_resource( &new_name, file )) {
		status_ = UPDATE_FAILED;
		return false;
	}

	// Need to move current file to .old suffix.
	JUTIL::DynamicString moved_name;
	if (!moved_name.write( "%s.old", file->get_string() )) {
		stack->log( "Failed to write new file name for updated file." );
		status_ = UPDATE_FAILED;
		return false;
	}

	// Check if current file even exists; user may have renamed EXE, for instance.
	FILE* moved;
	errno_t err = fopen_s( &moved, file->get_string(), "r" );
	if (moved != nullptr) {
		fclose( moved );

		// File is there, now move it.
		if (!MoveFile( file->get_string(), moved_name.get_string() )) {
			stack->log( "Failed to move updated file." );
			status_ = UPDATE_FAILED;
			return false;
		}
	}

	// Now move the new one to the original spot.
	if (!MoveFile( new_name.get_string(), file->get_string() )) {
		stack->log( "Failed to rename updated file to original name." );
		status_ = UPDATE_FAILED;
		return false;
	}
	return true;
}
