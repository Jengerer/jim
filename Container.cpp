#include "Container.h"

Container::Container( float x, float y ) : Component( x, y )
{
	// Container created.
}

Container::~Container()
{
	// Container destroyed.
}

void Container::updatePosition()
{
	// Position updated.
}

void Container::add( Component* component )
{
	size_t mid = 0;
	size_t min = 0, max = components_.size() - 1;

	// Loop through if not empty.
	if (!components_.empty()) {
		while (min <= max) {
			mid = (min + max) >> 1;

			Component* current = components_[mid];
			if (component < current) {
				max = mid - 1;
			}
			else if (component > current) {
				min = mid + 1;
			}
			else {
				return;
			}
		}

		vector<Component*>::iterator position = components_.begin() + mid;
		if (component < *position) {
			components_.insert( position, component );
		}
		else {
			components_.insert( position+1, component );
		}
	}
	else {
		components_.push_back( component );
	}
}

void Container::remove( Component* component )
{
	// Don't loop over empty set.
	if (components_.empty())
		return;

	size_t mid = 0;
	size_t min = 0, max = components_.size() - 1;
	while (min <= max) {
		mid = (min + max) >> 1;

		Component* current = components_[mid];
		if (component < current) {
			max = mid - 1;
		}
		else if (component > current) {
			min = mid + 1;
		} 
		else {
			break;
		}
	}

	if (components_[mid] == component) {
		components_.erase( components_.begin() + mid );
	}
	else {
		throw Exception( "Failed to find component within list." );
	}
}
