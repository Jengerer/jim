#pragma once

#include "Layout.h"
#include "Component.h"

enum EVerticalAlignType {
	ALIGN_TOP,
	ALIGN_MIDDLE,
	ALIGN_BOTTOM
};

class HorizontalLayout: public Layout
{

public:

	HorizontalLayout( void );
	virtual ~HorizontalLayout( void );

	// Packs elements into a horizontal layout.
	virtual void Pack( void );
	void SetMinimumHeight( int minimumHeight );

	void				SetAlignType( EVerticalAlignType alignType );
	EVerticalAlignType	GetAlignType( void ) const;

private:

	int minimumHeight_;
	EVerticalAlignType alignType_;


};
