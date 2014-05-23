#ifndef ITEM_DECORATOR_HPP
#define ITEM_DECORATOR_HPP

#include <jui/gfx/font_factory.hpp>
#include <jui/gfx/text.hpp>
#include <jui/layout/component.hpp>
#include "item.hpp"
#include "resource_loader_interface.hpp"

/*
 * Class for rendering an item in a slot view.
 */
class ItemDecorator: public JUI::Component
{

public:

	ItemDecorator( void );
	~ItemDecorator( void );

	// Handling item generic textures.
	static bool download_resources( ResourceLoaderInterface* loader );
	static bool precache( JUI::Graphics2D* graphics );
	static void release( void );

	// Update the item we're drawing.
	void set_item( const Item* item );

	// Rendering call.
	virtual void draw( JUI::Graphics2D* graphics );

private:

	// Item we're drawing and cached information about it.
	const Item* item_;

	// Static members for item rendering.
	static JUI::FileTexture* splat_;
	static JUI::FileTexture* splat_first_;
	static JUI::FileTexture* splat_second_;
	static JUI::FileTexture* spill_;
	static JUI::FileTexture* spill_first_;
	static JUI::FileTexture* spill_second_;

	// Text rendering shared.
	static JUI::Text* crate_text_;
	static JUI::Text* equipped_text_;

};

#endif // ITEM_DECORATOR_HPP