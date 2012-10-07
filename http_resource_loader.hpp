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

	HttpResourceLoader( const JUTIL::ConstantString& url, JUI::FileDownloader* downloader );

	// Resource loader interface function.
	bool get_resource( const JUTIL::ConstantString& file, const JUTIL::ConstantString& target ) const;

private:

	// Internal source management.
	void set_downloader( JUI::FileDownloader* downloader );
	void set_url( const JUTIL::ConstantString& url );

private:

	JUI::FileDownloader* downloader_;
	JUTIL::ConstantString url_;

};

#endif // HTTP_RESOURCE_LOADER_H