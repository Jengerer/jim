#ifndef SLOT_GRID_VIEW_HPP
#define SLOT_GRID_VIEW_HPP

#include <containers/vector.hpp>

#include <jui/layout/grid_layout.hpp>
#include <jui/io/mouse.hpp>

#include "slot_array.hpp"
#include "slot_view.hpp"

/*
 * Visual component for slot grid.
 */
class SlotGridView : public JUI::GridLayout, public SlotArrayListener
{
public:

	// Creation.
	SlotGridView( void );
	virtual ~SlotGridView( void );

	// Slot management.
	bool set_grid_size( unsigned int width, unsigned int height );

	// UI handling.
	const SlotView* get_slot_view( unsigned int index ) const;
	bool get_touching_index( const JUI::Mouse* mouse, unsigned int* index ) const;

	// Array listener interface.
	virtual bool on_slot_updated( unsigned int index, Slot* slot );

private:

	JUTIL::Vector<SlotView*> slot_views_;

};

#endif // SLOT_GRID_VIEW_HPP