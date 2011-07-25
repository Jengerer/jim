#include "file_texture.h"

FileTexture::FileTexture( const std::string& filename, const std::string& url )
{
	filename_ = filename;
	url_ = url;
}

const std::string& FileTexture::get_url() const
{
	return url_;
}

const std::string& FileTexture::get_filename() const
{
	return filename_;
}