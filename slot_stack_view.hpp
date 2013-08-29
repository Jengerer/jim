#ifndef SLOT_STACK_VIEW_HPP
#define SLOT_STACK_VIEW_HPP

#include "slot_view.hpp"
#include "slot_array_interface.hpp"
#include "slot_array_listener.hpp"

/*
 * Single slot view for multiple items for selection stack.
 */
class SlotStackView
    : public SlotView,
      public SlotArrayListener
{

public:

    SlotStackView( SlotArrayInterface* slot_array );
    virtual ~SlotStackView( void );

    // Array listener interface.
    virtual bool on_slot_updated( unsigned int index, const Slot* slot );

private:

    SlotArrayInterface* slot_array_;

};

#endif // SLOT_STACK_VIEW_HPP