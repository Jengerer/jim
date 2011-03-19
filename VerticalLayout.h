#pragma once

#include "Layout.h"
#include "Component.h"

class VerticalLayout: public Layout
{
public:
	VerticalLayout();
	virtual ~VerticalLayout();

	// Packs elements into a horizontal layout.
	virtual void pack();

	// Repositions all elements.
	virtual void updatePosition();
};