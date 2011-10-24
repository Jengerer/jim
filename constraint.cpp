#include "constraint.h"

Constraint::Constraint( Component* component, float localX, float localY )
{
	component_ = component;
	set_constraint( localX, localY );
}

void Constraint::set_constraint( float localX, float localY )
{
	localX_ = localX;
	localY_ = localY;
}

Component* Constraint::get_component() const
{
	return component_;
}

float Constraint::get_constraint_x() const
{
	return localX_;
}

float Constraint::get_constraint_y() const
{
	return localY_;
}