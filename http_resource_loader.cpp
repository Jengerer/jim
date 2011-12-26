#include "http_resource_loader.h"

/*
 * Resource loader constructor.
 */
HttpResourceLoader::HttpResourceLoader( const std::string& url )
{
	set_url( url );
}

/*
 * Get a file, or retrieve it from the target.
 */
void HttpResourceLoader::get_resource( const std::string& file, const std::string& target ) const
{
	FileDownloader::check_and_get( file, url_ + target );
}

/*
 * Set base URL for targets.
 */
void HttpResourceLoader::set_url( const std::string& url )
{
	url_ = url;
}