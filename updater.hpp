#ifndef UPDATER_HPP
#define UPDATER_HPP

#include <jui/net/file_downloader.hpp>
#include "http_resource_loader.hpp"

// Return status values for update version check.
enum UpdateCheckResult
{
	CHECK_FAILED,
	UP_TO_DATE,
	UPDATE_REQUIRED
};

// Return status for updater.
enum UpdateStatus
{
	UPDATE_PENDING,
	UPDATE_FAILED,
	UPDATE_SUCCESSFUL
};

/*
 * Class that updates the libraries and the item manager.
 */
class Updater
{

public:

	Updater( JUI::FileDownloader* downloader, HttpResourceLoader* site_loader, const JUTIL::String* jim_version );
	~Updater( void );

	// Generic update handling function.
	UpdateCheckResult check_version_numbers( void );
	bool remove_old_files( void );
	bool update( void );
	bool run_new_version( void );
	UpdateStatus get_update_status( void );

private:

	// Compare version numbers and move/update if out of date.
	bool update_file( const JUTIL::String* file );

private:

	JUI::FileDownloader* downloader_;
	HttpResourceLoader* site_loader_;

	// Version string from item manager.
	const JUTIL::String* jim_version_;

	// Information about whether each component is up to date.
	UpdateStatus status_;
	bool is_jutil_latest_;
	bool is_jui_latest_;
	bool is_jim_latest_;

};

#endif // UPDATER_HPP