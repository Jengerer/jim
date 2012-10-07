#ifndef RESOURCE_LOADER_INTERFACE_HPP
#define RESOURCE_LOADER_INTERFACE_HPP

#include <string/constant_string.hpp>

/*
 * Interface for loading an asset from a source.
 */
class ResourceLoaderInterface
{

public:

	virtual bool get_resource( const JUTIL::ConstantString& file, const JUTIL::ConstantString& target ) const = 0;

};

#endif // RESOURCE_LOADER_INTERFACE_HPP