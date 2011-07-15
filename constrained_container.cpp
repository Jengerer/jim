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

Constraint* ConstrainedContainer::SetConstraint( Component* child, float localX, float localY )
{
	Constraint* constraint = nullptr;
	auto i = constraints_.find( child );
	if (i != constraints_.end()) {
		constraint = i->second;
		constraint->SetConstraint( localX, localY );
	}
	else {
		constraint = new Constraint( child, localX, localY );
		constraints_[child] = constraint;
	}

	ApplyConstraint( constraint );
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

void ConstrainedContainer::ApplyConstraint( Constraint* constraint )
{
	Component* component = constraint->GetComponent();
	component->SetPosition( GetX() + constraint->GetConstraintX(),
		GetY() + constraint->GetConstraintY() );
}

void ConstrainedContainer::ApplyConstraints()
{
	for (auto i = constraints_.begin(); i != constraints_.end(); ++i) {
		ApplyConstraint( i->second );
	}
}