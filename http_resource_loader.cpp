#include "http_resource_loader.hpp"

/*
 * Resource loader constructor.
 */
HttpResourceLoader::HttpResourceLoader( const JUTIL::String* url, JUI::FileDownloader* downloader )
{
	set_url( url );
	set_downloader( downloader );
}

/*
 * Get a file, or retrieve it from the target.
 */
bool HttpResourceLoader::get_resource( const JUTIL::String* file, const JUTIL::String* target ) const
{
    // Generate full URL.
    JUTIL::DynamicString full_url;
    if (!full_url.write( "%s/%s", url_->get_string(), target->get_string() )) {
        return false;
    }

	return downloader_->check_and_get( file, &full_url );
}

/*
 * Get a file, always updating it from the url.
 */
bool HttpResourceLoader::update_resource( const JUTIL::String* file, const JUTIL::String* target ) const
{
    // Generate full URL.
    JUTIL::DynamicString full_url;
    if (!full_url.write( "%s/%s", url_->get_string(), target->get_string() )) {
        return false;
    }

	return downloader_->get( file, &full_url );
}

/*
 * Set file downloader.
 */
void HttpResourceLoader::set_downloader( JUI::FileDownloader* downloader )
{
	downloader_ = downloader;
}

/*
 * Set base URL for targets.
 */
void HttpResourceLoader::set_url( const JUTIL::String* url )
{
	url_ = url;
}