#ifndef CONSTRAINT_CONTAINER_H
#define CONSTRAINT_CONTAINER_H

#include <map>
#include "container.h"

struct Constraint
{
	float localX;
	float localY;
};

class ConstrainedContainer : public Container
{
public:

	ConstrainedContainer( float x = 0.0f, float y = 0.0f );
	virtual ~ConstrainedContainer();

	// Component override.
	virtual void SetPosition( float x, float y );

	// Container overrides.
	virtual void Remove( Component* child );
	
	// Constraint management.
	void SetConstraint( Component* child, float localX, float localY );
	void RemoveConstraint( Component* child );
	void RemoveAllConstraints();
	void ApplyConstraints();

private:

	std::map<Component*, Constraint*> constraints_;

};

#endif // CONSTRAINT_CONTAINER_H