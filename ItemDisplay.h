#pragma once

#include "Container.h"
#include "Item.h"
#include "Font.h"

#define DISPLAY_PADDING 10
#define DISPLAY_SPACING	10
#define DISPLAY_WIDTH	200
#define DISPLAY_ALPHA_SPEED	30
#define DISPLAY_ALPHA_MAX	225
#define DISPLAY_NAME_COLOUR	D3DCOLOR_XRGB( 255, 255, 0 )

class ItemDisplay: public Container
{
public:
	ItemDisplay();
	virtual ~ItemDisplay();

	// Drawing functions.
	virtual void draw( DirectX* directX );

	// Item functions.
	void setItem( Item *item );
	void setActive( bool isActive );

	// Drawing objects.
	static Font *nameFont;
	static Font *infoFont;

private:
	Item *item_;
	bool isActive_;
	int alpha_;
};