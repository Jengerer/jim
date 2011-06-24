#pragma once

#include "layout.h"
#include "component.h"

enum EHorizontalAlignType {
	ALIGN_LEFT,
	ALIGN_CENTER,
	ALIGN_RIGHT
};

class VerticalLayout : public Layout
{

public:

	VerticalLayout( unsigned int spacing = 0, EHorizontalAlignType alignType = ALIGN_CENTER );

	// Packs elements into a horizontal layout.
	virtual void Pack( void );
	void SetMinimumWidth( int minimumWidth );

	void					SetAlignType( EHorizontalAlignType alignType );
	EHorizontalAlignType	GetAlignType( void ) const;

private:

	int minimumWidth_;
	EHorizontalAlignType alignType_;


};