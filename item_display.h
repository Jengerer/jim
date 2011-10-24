#pragma once

#include "container.h"
#include "font.h"
#include "graphics_2d.h"
#include "item.h"
#include "rounded_container.h"
#include "text.h"
#include "wrapped_text.h"
#include "vertical_layout.h"

class ItemDisplay : public RoundedRectangleContainer
{

public:

	ItemDisplay( void );
	virtual ~ItemDisplay( void );

	// Drawing functions.
	void			UpdateDisplay();
	void			UpdateAlpha();
	virtual void	pack();

	// Item functions.
	const Item*		get_item( void ) const;
	void			set_item( const Item *item );

	const std::string&	get_name( void ) const;

	bool			IsActive( void ) const;
	void			SetActive( bool is_active );

	static void		precache();
	static void		Release();

private:

	void			SetName( const std::string& name );

protected:

	static Font *nameFont_;
	static Font *infoFont_;

private:

	bool		is_active_;
	const Item	*item_;
	std::string	itemName_;

	VerticalLayout		*textLayout_;
	WrappedText			*nameText_;
	WrappedText			*infoText_;

};