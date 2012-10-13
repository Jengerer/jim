#ifndef SLOT_GRID_VIEW_H
#define SLOT_GRID_VIEW_H

#include <containers/vector.hpp>

#include <jui/layout/grid_layout.hpp>
#include <jui/io/mouse.hpp>

#include "slot_array.hpp"
#include "slot_view.hpp"

/*
 * Visual component for slot grid.
 */
class SlotGridView : public JUI::GridLayout
{
public:

	// Creation.
	SlotGridView( unsigned int grid_width,
		unsigned int spacing );

	// Slot management.
	bool add_slots( const SlotArray* slots );

	// UI handling.
	SlotView* get_touching_slot( JUI::Mouse* mouse ) const;

	// Slot view management.
	void	set_enabled( bool is_enabled ) const;
	void	disable_full( void ) const;

private:

	// Slot management.
	bool add_slot( Slot* slot );

private:

	JUTIL::Vector<SlotView*> slot_views_;

};

#endif // SLOT_GRID_VIEW_H