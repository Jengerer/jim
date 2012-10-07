#include "http_resource_loader.hpp"

/*
 * Resource loader constructor.
 */
HttpResourceLoader::HttpResourceLoader( const JUTIL::ConstantString& url, JUI::FileDownloader* downloader )
{
	set_url( url );
	set_downloader( downloader );
}

/*
 * Get a file, or retrieve it from the target.
 */
bool HttpResourceLoader::get_resource( const JUTIL::ConstantString& file, const JUTIL::ConstantString& target ) const
{
	downloader_->check_and_get( file, url_ + target );
}

/*
 * Set file downloader.
 */
void HttpResourceLoader::set_downloader( FileDownloader* downloader )
{
	downloader_ = downloader;
}

/*
 * Set base URL for targets.
 */
void HttpResourceLoader::set_url( const std::string& url )
{
	url_ = url;
}