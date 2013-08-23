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
	SlotGridView( unsigned int grid_width, unsigned int grid_height );
	virtual ~SlotGridView( void );

	// Slot grid functions.
	bool initialize( void );
	unsigned int get_grid_width( void ) const;
	unsigned int get_grid_height( void ) const;
	unsigned int get_grid_size( void ) const;

	// UI handling.
	const SlotView* get_slot_view( unsigned int index ) const;
	bool get_touching_index( const JUI::Mouse* mouse, unsigned int* index ) const;

	// Array listener interface.
	virtual bool on_slot_updated( unsigned int index, const Slot* slot );

private:

	JUTIL::Vector<SlotView*> slot_views_;
	unsigned int grid_width_;
	unsigned int grid_height_;

};

#endif // SLOT_GRID_VIEW_HPP