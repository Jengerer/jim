#ifndef IRESOURCE_LOADER_H
#define IRESOURCE_LOADER_H

#include <string>

class IResourceLoader
{

public:

	virtual void get_resource( const std::string& file, const std::string& target ) const = 0;

};

#endif // IRESOURCE_LOADER_H