#ifndef RESOURCE_LOADER_INTERFACE_HPP
#define RESOURCE_LOADER_INTERFACE_HPP

#include <string/string.hpp>

/*
 * Interface for loading an asset from a source.
 */
class ResourceLoaderInterface
{

public:

	virtual bool get_resource( const JUTIL::String* file, const JUTIL::String* target ) const = 0;

};

#endif // RESOURCE_LOADER_INTERFACE_HPP