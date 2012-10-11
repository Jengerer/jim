#ifndef HTTP_RESOURCE_LOADER_H
#define HTTP_RESOURCE_LOADER_H

#include <string>
#include <hash_map>
#include <jui/net/file_downloader.hpp>
#include <string/string_hasher.hpp>

#include "resource_loader_interface.hpp"

/*
 * Resource loader from HTTP source.
 */
class HttpResourceLoader : public ResourceLoaderInterface
{

public:

	HttpResourceLoader( const JUTIL::String* url, JUI::FileDownloader* downloader );

	// Resource loader interface function.
	bool get_resource( const JUTIL::String* file, const JUTIL::String* target ) const;

private:

	// Internal source management.
	void set_downloader( JUI::FileDownloader* downloader );
	void set_url( const JUTIL::String* url );

private:

	JUI::FileDownloader* downloader_;
	const JUTIL::String* url_;

};

#endif // HTTP_RESOURCE_LOADER_H