#ifndef SLOT_ARRAY_VIEW_LISTENER_HPP
#define SLOT_ARRAY_VIEW_LISTENER_HPP

#include "slot_array_interface.hpp"
#include <jui/io/mouse.hpp>

/*
 * Interface for handling UI events with a slot array view.
 */
class SlotArrayViewListener
{

public:

    virtual bool on_slot_hovered( SlotArrayInterface* slot_array, unsigned int index ) = 0;
    virtual bool on_slot_clicked( JUI::Mouse* mouse, SlotArrayInterface* slot_array, unsigned int index ) = 0;
    virtual bool on_slot_released( SlotArrayInterface* slot_array, unsigned int index ) = 0;

};

#endif // SLOT_ARRAY_VIEW_LISTENER_HPP