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
	virtual void SetPosition( float x, float y );

	// Container overrides.
	virtual void remove( Component* child );
	
	// Constraint management.
	Constraint* SetConstraint( Component* child, float localX, float localY );
	void RemoveConstraint( Component* child );
	void RemoveAllConstraints();
	void ApplyConstraint( Constraint* constraint );
	void ApplyConstraints();

private:

	std::map<Component*, Constraint*> constraints_;

};

#endif // CONSTRAINT_CONTAINER_H