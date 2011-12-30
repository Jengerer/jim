#ifndef HTTP_RESOURCE_LOADER_H
#define HTTP_RESOURCE_LOADER_H

#include <string>
#include <hash_map>
#include <jui/file_downloader.h>
#include <jui/string_hasher.h>

#include "iresource_loader.h"

// Map for file/target pairs.
typedef std::hash_map<std::string, std::string, StringHasher> DownloadableFiles;

class HttpResourceLoader : public IResourceLoader
{

public:

	HttpResourceLoader( const std::string& url, FileDownloader* downloader );

	// Resource loader interface function.
	void get_resource( const std::string& file, const std::string& target ) const;

private:

	void set_downloader( FileDownloader* downloader );
	void set_url( const std::string& url );

private:

	FileDownloader* downloader_;
	std::string url_;

};

#endif // HTTP_RESOURCE_LOADER_H