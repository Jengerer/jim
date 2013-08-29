#include "slot_stack_view.hpp"

SlotStackView::SlotStackView( SlotArrayInterface* slot_array )
    : slot_array_( slot_array )
{
}

SlotStackView::~SlotStackView( void )
{
}

/*
 * Handle slot container update event.
 */
bool SlotStackView::on_slot_updated( unsigned int index, const Slot* slot )
{
    // Only update top slot.
    if (index == 0) {
        if (!update( slot )) {
            return false;
        }
    }
    return true;
}