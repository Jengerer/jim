#pragma once

#include "Layout.h"
#include "Component.h"

class HorizontalLayout: public Layout
{
public:
	HorizontalLayout();
	virtual ~HorizontalLayout();

	// Packs elements into a horizontal layout.
	virtual void pack();

	// Repositions all elements.
	virtual void updatePosition();
};