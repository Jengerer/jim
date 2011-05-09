#pragma once

#include "Layout.h"
#include "Component.h"

class VerticalLayout : public Layout
{

public:

	VerticalLayout( void );
	virtual ~VerticalLayout( void );

	// Packs elements into a horizontal layout.
	virtual void Pack( void );

	// Repositions all elements.
	virtual void UpdatePosition( void );

};