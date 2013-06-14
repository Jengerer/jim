#ifndef SLOT_GRID_PAGES_HPP
#define SLOT_GRID_PAGES_HPP

#include <jui/layout/constrained_container.hpp>
#include <jui/layout/horizontal_layout.hpp>
#include <jui/io/mouse.hpp>

#include "slot_grid_view.hpp"
#include "slot_array_listener.hpp"

/*
 * Extension of slot grid view to switch between multiple pages.
 */
class SlotBookView : public SlotGridView, public SlotArrayListener
{

public:

	SlotBookView( void );
	virtual ~SlotBookView( void );

	// Mouse event handling handling.
	unsigned int get_touching_index( JUI::Mouse* mouse ) const;

	// Slot array listener handlers.
	virtual bool on_slot_updated( unsigned int index, Slot* slot );

private:

	// Navigation functions.
	void set_active_page( unsigned int page );
	void set_view_offset( float view_offset );

};

#endif // SLOT_GRID_PAGES_HPP