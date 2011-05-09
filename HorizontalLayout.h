#pragma once

#include "Layout.h"
#include "Component.h"

class HorizontalLayout: public Layout
{
public:
	HorizontalLayout( void );
	virtual ~HorizontalLayout( void );

	// Packs elements into a horizontal layout.
	virtual void Pack( void );

	// Repositions all elements.
	virtual void UpdatePosition( void );
};