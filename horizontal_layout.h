#pragma once

#include "layout.h"
#include "component.h"

enum EVerticalAlignType {
	ALIGN_TOP,
	ALIGN_MIDDLE,
	ALIGN_BOTTOM
};

class HorizontalLayout: public Layout
{

public:

	HorizontalLayout( unsigned int spacing = 0, EVerticalAlignType alignType = ALIGN_MIDDLE );

	// Packs elements into a horizontal layout.
	virtual void Pack( void );
	void SetMinimumHeight( int minimumHeight );

	void				SetAlignType( EVerticalAlignType alignType );
	EVerticalAlignType	GetAlignType( void ) const;

private:

	int minimumHeight_;
	EVerticalAlignType alignType_;


};
