#include "constrained_container.h"

ConstrainedContainer::ConstrainedContainer( float x, float y ) : Container( x, y )
{
	// Constraint container.
}

ConstrainedContainer::~ConstrainedContainer()
{
	RemoveAllConstraints();
}

void ConstrainedContainer::SetPosition( float x, float y )
{
	Component::SetPosition( x, y );
	ApplyConstraints();
}

void ConstrainedContainer::Remove( Component* child )
{
	Container::Remove( child );
	RemoveConstraint( child );
}

void ConstrainedContainer::SetConstraint( Component* child, float localX, float localY )
{
	auto i = constraints_.find( child );
	if (i != constraints_.end()) {
		Constraint* constraint = i->second;
		constraint->localX = localX;
		constraint->localY = localY;
	}
	else {
		Constraint* constraint = new Constraint();
		constraint->localX = localX;
		constraint->localY = localY;
		constraints_[child] = constraint;
	}

	child->SetPosition( GetX() + localX, GetY() + localY );
}

void ConstrainedContainer::RemoveConstraint( Component* child )
{
	auto i = constraints_.find( child );
	if (i != constraints_.end()) {
		constraints_.erase( i );
	}
}

void ConstrainedContainer::RemoveAllConstraints()
{
	for (auto i = constraints_.begin(); i != constraints_.end(); i = constraints_.erase( i )) {
		delete i->second;
	}
}

void ConstrainedContainer::ApplyConstraints()
{
	for (auto i = constraints_.begin(); i != constraints_.end(); ++i) {
		Component* component = i->first;
		Constraint* constraint = i->second;
		component->SetPosition( GetX() + constraint->localX, GetY() + constraint->localY );
	}
}