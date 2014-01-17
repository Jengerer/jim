#ifndef SLOT_GRID_VIEW_HPP
#define SLOT_GRID_VIEW_HPP

#include <containers/vector.hpp>

#include <jui/layout/grid_layout.hpp>
#include <jui/io/mouse.hpp>
#include <jui/io/mouse_handler_interface.hpp>

#include "slot_array.hpp"
#include "slot_array_view_listener.hpp"
#include "slot_view.hpp"

/*
 * Visual component for slot grid.
 */
class SlotGridView
    : public JUI::GridLayout,
      public SlotArrayListener,
      public JUI::MouseHandlerInterface
{
public:

	// Creation.
	SlotGridView( SlotArrayInterface* slot_array, unsigned int grid_width, unsigned int grid_height );
	virtual ~SlotGridView( void );

    // Listener setting.
    void set_listener( SlotArrayViewListener* listener );

	// Slot grid functions.
	bool initialize( void );
	unsigned int get_grid_width( void ) const;
	unsigned int get_grid_height( void ) const;
	unsigned int get_grid_size( void ) const;

	// UI handling.
	const SlotView* get_slot_view( unsigned int index ) const;
    virtual JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );
    virtual JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
    virtual JUI::IOResult on_mouse_released( JUI::Mouse* mouse );

	// Array listener interface.
	virtual void on_slot_updated( unsigned int index, const Slot* slot );

protected:

    // Get the index of slot the mouse is touching, if any.
    virtual bool get_touching_index( const JUI::Mouse* mouse, unsigned int* index ) const;

protected:

    // Slot array interface for querying slots.
	SlotArrayInterface* slot_array_;

private:

    // UI event listener.
    SlotArrayViewListener* listener_;

    // Slot view parameters.
	JUTIL::Vector<SlotView*> slot_views_;
	unsigned int grid_width_;
	unsigned int grid_height_;

};

#endif // SLOT_GRID_VIEW_HPP