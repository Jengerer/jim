#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "component.h"

class Constraint
{
public:

	Constraint( Component* component, float localX, float localY );
	void SetConstraint( float localX, float localY );

	Component* GetComponent() const;
	float GetConstraintX() const;
	float GetConstraintY() const;

private:

	Component* component_;
	float localX_;
	float localY_;

};

#endif // CONSTRAINT_H