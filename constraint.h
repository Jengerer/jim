#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "component.h"

class Constraint
{
public:

	Constraint( Component* component, float localX, float localY );
	void set_constraint( float localX, float localY );

	Component* get_component() const;
	float get_constraint_x() const;
	float get_constraint_y() const;

private:

	Component* component_;
	float localX_;
	float localY_;

};

#endif // CONSTRAINT_H