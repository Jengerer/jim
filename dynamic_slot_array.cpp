#include "dynamic_slot_array.hpp"

/*
 * Dynamic slot book constructor.
 */
DynamicSlotArray::DynamicSlotArray( void )
{
}

/*
 * Attempts to add an item to the dynamic slot book.
 * This function assumes that the dynamic slot book is compressed
 * and there are no gaps between full slots.
 * Returns true if able to add the slot.
 */
bool DynamicSlotArray::push_item( Item* item )
{
    // Check the last page.
    unsigned int end = get_size();
    if (!set_size( end + 1 )) {
        return false;
    }
    set_item( end, item );
    return true;
}

/*
 * Compresses the slot book to remove gaps between occupied slots.
 * Returns true if successful, false otherwise.
 */
bool DynamicSlotArray::compress_slots( void )
{
    // Find the first empty slot.
    unsigned int i;
    unsigned int end = get_size();
    unsigned int empty = end;
    for (i = 0; i < end; ++i) {
        Item* item = get_item( i );
        if (item == nullptr) {
            empty = i;
            break;
        }
    }

    // Shift all items left if we have an empty slot.
    if (empty != end) {
        for (i = empty + 1; i < end; ++i) {
            Item* item = get_item( i );

            // Move item to empty slot if there is one.
            if (item != nullptr) {
                set_item( empty, item );
                set_item( i, nullptr );
                ++empty;
            }
        }
    }

    // Resize to minimum.
    if (!set_size( empty )) {
        return false;
    }
    return true;
}