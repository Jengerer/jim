#ifndef FILE_TEXTURE_H
#define FILE_TEXTURE_H

#include "texture.h"
#include <string>

class FileTexture : public Texture
{
	
public:

	FileTexture( const std::string& filename, const std::string& url );

	// File attributes.
	const std::string& get_url() const;
	const std::string& get_filename() const;

private:

	std::string filename_;
	std::string url_;

};

#endif // FILE_TEXTURE_H