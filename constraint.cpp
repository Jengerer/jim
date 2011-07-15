#include "constraint.h"

Constraint::Constraint( Component* component, float localX, float localY )
{
	component_ = component;
	SetConstraint( localX, localY );
}

void Constraint::SetConstraint( float localX, float localY )
{
	localX_ = localX;
	localY_ = localY;
}

Component* Constraint::GetComponent() const
{
	return component_;
}

float Constraint::GetConstraintX() const
{
	return localX_;
}

float Constraint::GetConstraintY() const
{
	return localY_;
}