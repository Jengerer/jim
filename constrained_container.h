#ifndef CONSTRAINT_CONTAINER_H
#define CONSTRAINT_CONTAINER_H

#include <map>
#include "constraint.h"
#include "container.h"

class ConstrainedContainer : public Container
{
public:

	ConstrainedContainer( float x = 0.0f, float y = 0.0f );
	virtual ~ConstrainedContainer();

	// Component override.
	virtual void set_position( float x, float y );

	// Container overrides.
	virtual void remove( Component* child );
	
	// Constraint management.
	Constraint* set_constraint( Component* child, float localX, float localY );
	void RemoveConstraint( Component* child );
	void RemoveAllConstraints();
	void apply_constraint( Constraint* constraint );
	void apply_constraints();

private:

	std::map<Component*, Constraint*> constraints_;

};

#endif // CONSTRAINT_CONTAINER_H