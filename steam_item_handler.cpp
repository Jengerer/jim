#include "steam_item_handler.hpp"
#include "serialized_buffer.hpp"
#include "jui/application/error_stack.hpp"

/*
 * Item and selection handler constructor.
 */
SteamItemHandler::SteamItemHandler( void )
{
	set_select_mode( SELECT_MODE_SINGLE );
}

/*
 * Item and selection handler destructor.
 */
SteamItemHandler::~SteamItemHandler( void )
{
	close_interfaces();
}

/*
 * Set an item as being selected.
 * Assumes it hasn't already been selected.
 * Returns true if selection succeeded, false otherwise.
 */
bool SteamItemHandler::select( SlotView* slot_view )
{
	// Error stack for error reporting.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Deselect all if we're not multi-selecting.
	if (get_select_mode() == SELECT_MODE_SINGLE) {
		deselect_all();
	}

    // Insert and toggle selection on slot.
	if (!selected_.insert( slot_view )) {
		stack->log( "Failed to add slot to selection." );
        return false;
    }
	slot_view->set_selected( true );
    return true;
}

/*
 * Deselect an item.
 */
void SteamItemHandler::deselect( SlotView* slot_view )
{
	selected_.remove( slot_view );
	slot_view->set_selected( false );
}

/*
 * Toggle selection on an item.
 * Returns true if selection succeeded, false otherwise.
 */
bool SteamItemHandler::toggle_select( SlotView* slot_view )
{
	if (is_selected( slot_view )) {
		deselect( slot_view );
	}
	else {
		if (!select( slot_view )) {
            return false;
        }
	}

    return true;
}

void SteamItemHandler::deselect_all( void )
{
    JUTIL::Set<SlotView*>::Iterator i;
    for (i = selected_.begin(); i.has_next(); i.next()) {
        SlotView* slot_view = i.get_value();
		slot_view->set_selected( false );
	}
    selected_.clear();
}

bool SteamItemHandler::is_selected( SlotView* slot_view ) const
{
	return selected_.contains( slot_view );
}

size_t SteamItemHandler::get_selected_count() const
{
	return selected_.get_length();
}

void SteamItemHandler::set_select_mode( SelectMode select_mode )
{
	select_mode_ = select_mode;
}

SelectMode SteamItemHandler::get_select_mode() const
{
	return select_mode_;
}

void SteamItemHandler::update_item( const Item* item ) const
{
	GCSetItemPosition_t message;
	message.itemID = item->get_unique_id();
	message.position = item->get_flags();

	// Send it.
	send_message( 
		static_cast<uint32>(GCSetItemPosition_t::k_iMessage), 
		static_cast<void*>(&message),
		sizeof(message) );
}

bool SteamItemHandler::is_selected_tradable( void ) const
{
    // Loop through all and check tradable.
    JUTIL::Set<SlotView*>::Iterator i;
    for (i = selected_.begin(); i.has_next(); i.next()) {
        SlotView* slot_view = i.get_value();
		Slot* slot = slot_view->get_slot();
		if (slot->has_item()) {
			Item* item = slot->get_item();

			// Check that the item can be traded.
			if (!item->is_tradable()) {
				return false;
			}
		}
	}

	return true;
}

bool SteamItemHandler::craft_selected( void )
{
	// Number of items to craft.
	uint16 item_count = get_selected_count();
	if (item_count != 0) {
		// Allocate and set.
        size_t message_size = sizeof(struct GCCraft_t) + sizeof(uint64) * item_count;
        JUTIL::ArrayBuilder<char> message;
        if (!message.set_size( message_size )) {
            return false;
        }

		// Values to fill in.
		uint16 id = 1;

		// Fill message.
		SerializedBuffer buffer( message.get_array() );
		buffer.write( &id, sizeof( id ) );
		buffer.push( sizeof(char[16]) );
		buffer.push( sizeof(uint16) );
		buffer.write( &item_count, sizeof( item_count ) );
	
		// Write all item IDs.
        JUTIL::Set<SlotView*>::Iterator i;
        for (i = selected_.begin(); i.has_next(); i.next()) {
			SlotView* slot_view = i.get_value();

            // Write item ID to buffer.
			Item* item = slot_view->get_slot()->get_item();
			uint64 item_id = item->get_unique_id();
			buffer.write( &item_id, sizeof( item_id ) );
		}

		// Deselect everything.
		deselect_all();

		// Send message.
		if (!send_message( GCCraft_t::k_iMessage, message.get_array(), message.get_size() )) {
            return false;
        }
	}

    return true;
}

void SteamItemHandler::equip_selected( EClassEquip whichClass, bool setEquip )
{
	// To be implemented.
}

bool SteamItemHandler::can_equip_selected( void ) const
{
    // Check that one is selected.
	if (get_selected_count() == 1) {
        // Get first.
        JUTIL::Set<SlotView*>::Iterator i = selected_.begin();
        SlotView* selected = i.get_value();
		Item* item = selected->get_slot()->get_item();
		return item->get_equip_class_count() != 0;
	}

	return false;
}