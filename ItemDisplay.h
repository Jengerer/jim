#pragma once

#include "Container.h"
#include "DirectX.h"
#include "Font.h"
#include "Item.h"
#include "Text.h"
#include "WrappedText.h"
#include "VerticalLayout.h"

#define ITEM_DISPLAY_PADDING		10
#define ITEM_DISPLAY_SPACING		10
#define ITEM_DISPLAY_RADIUS			5
#define ITEM_DISPLAY_WIDTH			200
#define ITEM_DISPLAY_ALPHA_SPEED	30
#define ITEM_DISPLAY_ALPHA_MAX		225
#define ITEM_DISPLAY_NAME_COLOUR	D3DCOLOR_XRGB( 255, 255, 0 )

class ItemDisplay: public Container
{

public:

	ItemDisplay( void );
	virtual ~ItemDisplay( void );

	// Drawing functions.
	virtual void	OnDraw( DirectX* directX );
	virtual void	UpdatePosition( void );
	void			UpdateAlpha( void );
	virtual void	Pack( void );

	// Item functions.
	const Item*		GetItem( void ) const;
	void			SetItem( const Item *item );

	const string&	GetName( void ) const;

	bool			IsActive( void ) const;
	void			SetActive( bool isActive );

	static void		Precache( DirectX *directX );
	static void		Release( void );

private:

	void			SetName( const string& name );

protected:

	static Font *nameFont_;
	static Font *infoFont_;

private:

	bool		isActive_;
	const Item	*item_;
	string		itemName_;

	VerticalLayout	*textLayout_;
	WrappedText		*nameText_;
	WrappedText		*infoText_;
	Texture			*roundedRect_;

};