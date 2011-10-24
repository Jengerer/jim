#include "constrained_container.h"

ConstrainedContainer::ConstrainedContainer( float x, float y ) : Container( x, y )
{
	// Constraint container.
}

ConstrainedContainer::~ConstrainedContainer()
{
	RemoveAllConstraints();
}

void ConstrainedContainer::set_position( float x, float y )
{
	Component::set_position( x, y );
	apply_constraints();
}

void ConstrainedContainer::remove( Component* child )
{
	Container::remove( child );
	RemoveConstraint( child );
}

Constraint* ConstrainedContainer::set_constraint( Component* child, float localX, float localY )
{
	Constraint* constraint = nullptr;
	auto i = constraints_.find( child );
	if (i != constraints_.end()) {
		constraint = i->second;
		constraint->set_constraint( localX, localY );
	}
	else {
		constraint = new Constraint( child, localX, localY );
		constraints_[child] = constraint;
	}

	apply_constraint( constraint );
	return constraint;
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

void ConstrainedContainer::apply_constraint( Constraint* constraint )
{
	Component* component = constraint->get_component();
	component->set_position( get_x() + constraint->get_constraint_x(),
		get_y() + constraint->get_constraint_y() );
}

void ConstrainedContainer::apply_constraints()
{
	for (auto i = constraints_.begin(); i != constraints_.end(); ++i) {
		apply_constraint( i->second );
	}
}