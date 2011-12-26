#ifndef ITEM_DISPLAY_H
#define ITEM_DISPLAY_H

#include <jui/container.h>
#include <jui/font.h>
#include <jui/graphics_2d.h>
#include <jui/text.h>
#include <jui/wrapped_text.h>
#include <jui/vertical_layout.h>

#include "item.h"
#include "rounded_container.h"

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
	static void		release();

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

#endif // ITEM_DISPLAY_H