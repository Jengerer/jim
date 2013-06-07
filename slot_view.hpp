#ifndef SLOT_VIEW_H
#define SLOT_VIEW_H

#include <jui/layout/constrained_container.hpp>
#include <jui/gfx/font_interface.hpp>
#include <jui/gfx/font_factory.hpp>
#include <jui/gfx/image.hpp>
#include <jui/gfx/cropped_image.hpp>
#include <jui/gfx/text.hpp>

#include "item.hpp"
#include "rounded_rectangle.hpp"
#include "slot.hpp"

class SlotView: public JUI::ConstrainedContainer
{

public:

	// Slot display attributes.
	static const unsigned int ENABLED_ALPHA  = 255;
	static const unsigned int DISABLED_ALPHA = 50;
	static const unsigned int DRAG_ALPHA     = 200;
	static const unsigned int SLOT_RADIUS    = 7;

	// Initialization and construction.
	SlotView( Slot* slot );
    bool initialize( void );

	// Over-ridden drawing function.
<<<<<<< HEAD
	bool update( Slot* slot );
=======
	void update();
	virtual void update_alpha( void );
>>>>>>> 234a885e662495aeef177dc4df240f7cf5eab3bb
	void draw( JUI::Graphics2D* graphics );

	// Getting the slot.
	Slot* get_slot( void ) const;
	void set_slot( Slot* slot );

	// Status functions.
	void set_selected( bool is_selected );
	bool is_selected( void ) const;
	void set_enabled( bool is_enabled );
	bool is_enabled( void ) const;

	// Precaching class-wide resources.
	static bool precache( JUI::Graphics2D* graphics );
	static void release( void );

private:
	
	// Font and text for indicating equipped.
	static JUI::FontInterface* equipped_font_;
	static JUI::Text* equipped_text_;

	static JUI::FontInterface* crate_font_;
	static JUI::Text* crate_text_;

protected:

	// Item icon image.
	JUTIL::Vector<JUI::CroppedImage*> item_images_;

private:

	// Rounded rectangle pointers.
	RoundedRectangle* slot_rectangle_;

<<<<<<< HEAD
	// Item icon image.
	JUI::Image* image_;

=======
	Slot* slot_;
	bool is_selected_;
>>>>>>> 234a885e662495aeef177dc4df240f7cf5eab3bb
	bool is_enabled_;

};

#endif // SLOT_VIEW_H