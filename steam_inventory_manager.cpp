#include "steam_inventory_manager.hpp"
#include "serialized_buffer.hpp"
#include "jui/application/error_stack.hpp"

/*
 * Item and selection handler constructor.
 */
SteamInventoryManager::SteamInventoryManager( void )
{
}

/*
 * Item and selection handler destructor.
 */
SteamInventoryManager::~SteamInventoryManager( void )
{
	close_interfaces();
}

/*
 * Set the interface for handling steam inventory events.
 */
void SteamInventoryManager::set_listener( SteamInventoryListener* listener )
{
	listener_ = listener;
}

/*
 * Set the pointer for the interface that will handle inventory-specific
 * Steam events.
 */
bool SteamInventoryManager::handle_callbacks( void )
{
	// Get error stack reference.
	JUI::ErrorStack *stack = JUI::ErrorStack::get_instance();

    CallbackMsg_t callback;
    if (get_callback( &callback )) {
		// Pass callback to handling function.
		unsigned int id = callback.m_iCallback;
		void* message = callback.m_pubParam;
		if (!handle_callback( id, message )) {
			stack->log( "Failed to handle received callback." );
			return false;
		}
		release_callback();
	}
    return true;
}

/*
 * Send new position to backend.
 */
bool SteamInventoryManager::move_item( const Item* item, unsigned int index ) const
{
#if defined(LOG_STEAM_MESSAGES)
	if (log_ != nullptr) {
		fprintf( log_, "Sent move for item %llu.\n", item->get_unique_id() );
		fflush( log_ );
	}
#endif

	// Generate new flags.
	uint32 flags = item->has_valid_inventory_flags() ? item->get_inventory_flags() : FL_ITEM_VALID;
	flags = (flags & FL_ITEM_NONPOSITION) | ((index + 1) & FL_ITEM_POSITION);
	
	// Create message and send.
	GCSetItemPosition_t message;
	message.itemID = item->get_unique_id();
	message.position = flags;
	return send_message( 
		static_cast<uint32>(GCSetItemPosition_t::k_iMessage), 
		static_cast<void*>(&message),
		sizeof(message) );
} 

/*
 * Delete a single item.
 */
bool SteamInventoryManager::delete_item( const Item* item ) const
{
#if defined(LOG_STEAM_MESSAGES)
	if (log_ != nullptr) {
		fprintf( log_, "Sent move for item %llu.\n", item->get_unique_id() );
		fflush( log_ );
	}
#endif

	GCDelete_t message;
	message.itemID = item->get_unique_id();
	return send_message(
		static_cast<uint32>(GCDelete_t::k_iMessage),
		&message,
		sizeof(message));
}

/*
 * Clear all items in the update message.
 */
void SteamInventoryManager::clear_item_updates( void )
{
	update_msg_.clear_item_data();
}

/*
 * Add an item to be updated.
 */
bool SteamInventoryManager::add_item_update( const Item* item )
{
	// Fill in the message.
	uint64 id = item->get_unique_id();
	uint32 inventory_flags = item->get_inventory_flags();
	CItemUpdate updated_item;
	updated_item.set_id( id );
	updated_item.set_position( inventory_flags );

	// SErialize and add to the message.
	int item_size = updated_item.ByteSize();
	JUTIL::ArrayBuilder<char> buffer;
	if (!buffer.set_size( item_size )) {
		return false;
	}
	char* buffer_data = buffer.get_array();
	updated_item.SerializeToArray( buffer_data, item_size );
	update_msg_.add_item_data( buffer_data, item_size );
	return true;
}

/*
 * Update a single item.
 */
bool SteamInventoryManager::push_item_updates( void )
{
	// Update message type used in a few places.
	uint32 update_message_id = k_EMsgGCUpdateItems | PROTOBUF_MESSAGE_FLAG;

	// Serialize and send.
	int message_size = update_msg_.ByteSize();
	JUTIL::ArrayBuilder<char> buffer;
	if (!buffer.set_size( message_size )) {
		return false;
	}
	char* buffer_data = buffer.get_array();
	update_msg_.SerializeToArray( buffer_data, message_size );
	return send_protobuf_message( 0, update_message_id, buffer_data, message_size );
}

/*
 * Set number of items to be sent to craft message.
 */
bool SteamInventoryManager::set_craft_size( unsigned int count )
{
	// Allocate space for base message and item IDs.
	unsigned int message_size = sizeof(GCCraft_t);
	message_size += sizeof(uint64) * count;
	if (!craft_buffer_.set_size( message_size )) {
		return false;
	}

	// Update blueprint name and item count.
	GCCraft_t* message = reinterpret_cast<GCCraft_t*>(craft_buffer_.get_array());
	message->blueprint = 0xFFFF;
	message->itemcount = count;

	return true;
}

/*
 * Add an item to the item craft buffer.
 */
void SteamInventoryManager::set_craft_item( unsigned int index, const Item* item )
{
	char* items_offset = craft_buffer_.get_array() + sizeof(GCCraft_t);
	uint64* items = reinterpret_cast<uint64*>(items_offset);
	items[index] = item->get_unique_id();
}

/*
 * Send the item IDs in the craft buffer array to be crafted.
 */
bool SteamInventoryManager::craft_items( void )
{
#if defined(LOG_STEAM_MESSAGES)
	if (log_ != nullptr) {
		fprintf( log_, "Sent craft message.\n" );
	}
#endif

	// Send message.
	if (!send_message( GCCraft_t::k_iMessage, craft_buffer_.get_array(), craft_buffer_.get_size() )) {
        return false;
    }

    return true;
}

/*
 * Handle a single callback message from Steam.
 */
bool SteamInventoryManager::handle_callback( uint32 id, void* message )
{
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

#if defined(LOG_STEAM_MESSAGES)
	static uint32 previous_id = 0xFFFFFFFF;
	if (log_ != nullptr && id != previous_id) {
		fprintf( log_, "Got callback with ID %d (0x%u).\n", id, id );
		fflush( log_ );
		previous_id = id;
	}
#endif

	switch (id) {
	case GCMessageAvailable_t::k_iCallback:
	{
		// Get size of message waiting.
		uint32 size;
		if (!has_message( &size )) {
			stack->log( "Message marked available, but none found." );
			return false;
		}
		
		// Get message from Steam.
		uint32 message_id;
		uint32 real_size;
		JUTIL::ArrayBuilder<uint8> buffer;
		if (!buffer.set_size( size )) {
			stack->log( "Failed to allocate buffer for game coordinator message." );
			return false;
		}
		if (!get_message( &message_id, buffer.get_array(), size, &real_size )) {
			stack->log( "Failed to retrieve message from Steam." );
			return false;
		}

		// Check if message is protobuf message.
		if ((message_id & PROTOBUF_MESSAGE_FLAG) == PROTOBUF_MESSAGE_FLAG) {
			// Clear flag to get real message ID.
			message_id &= ~PROTOBUF_MESSAGE_FLAG;

			// First get the protobuf struct header.
			SerializedBuffer header_buffer( buffer.get_array() );
			GCProtobufHeader_t* header_struct = header_buffer.get<GCProtobufHeader_t>();
			uint32 header_size = header_struct->m_cubProtobufHeader;

			// Now get the real protobuf header.
			CMsgProtoBufHeader header_msg;
			void* header_bytes = header_buffer.here();
			if (!header_msg.ParseFromArray( header_bytes, header_size )) {
				stack->log( "Failed to parse game coordinator message." );
				return false;
			}
			header_buffer.push( header_size );

			// Check if we can set target ID.
			uint64 job_id_source;
			if (header_msg.has_job_id_source()) {
				job_id_source = header_msg.job_id_source();
			}
			else {
				job_id_source = 0;
			}

			// Pass message to protobuf handler.
			uint32 body_size = size - sizeof(GCProtobufHeader_t) - header_size;
 			if (!handle_protobuf( message_id, header_buffer.here(), body_size, job_id_source )) {
				stack->log( "Failed to handle protobuf game coordinator message." );
				return false;
			}
		}
		else {
			// Handle non-protobuf message.
			if (!handle_message( message_id, buffer.get_array() )) {
				stack->log( "Failed to handle regular message." );
				return false;
			}
		}
		break;
	}

	case LogonFailure_t::k_iCallback:
	case IPCFailure_t::k_iCallback:
		stack->log( "Lost connection to Steam. Please check that the Steam network is online and restart the item manager." );
		return false;
		break;

	default:
		break;
	}

    return true;
}

/*
 * Handle non-protobuf Steam message.
 */
bool SteamInventoryManager::handle_message( uint32 id, void* message )
{
    // Handle by message ID.
	switch (id) {
		case GCCraftResponse_t::k_iMessage:
		{
			GCCraftResponse_t* response = reinterpret_cast<GCCraftResponse_t*>(message);
			if (response->blueprint == 0xFFFF) {
				listener_->on_craft_failed();
			}
			break;
		}
	}

    // No issues, hooray!
    return true;
}

#define WRITE_TO_FILE
#if defined(READ_FROM_FILE) || defined(WRITE_TO_FILE)
#include <fstream>
#endif

/*
 * Handle protobuf Steam message. Prepare your anus.
 */
bool SteamInventoryManager::handle_protobuf( uint32 id, void* message, uint32 size, uint64 job_id_source )
{
    // Get error stack for logging.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Handle message from ID.
	switch (id) {
	case k_ESOMsg_CacheSubscribed:
		{
			// Get message.
			CMsgSOCacheSubscribed cache_msg;
#if !defined(READ_FROM_FILE)
			if (!cache_msg.ParseFromArray( message, size )) {
                return false;
            }
#else
			std::ifstream in_file( "inventory.bin", std::ios::binary );
			if (!cache_msg.ParseFromIstream( &in_file )) {
				return false;
			}
			in_file.close();
#endif

#if defined(WRITE_TO_FILE)
			std::ofstream out_file("inventory.bin", std::ios::binary );
			if (!cache_msg.SerializeToOstream( &out_file )) {
				stack->log( "Failed to serialize inventory to file." );
				return false;
			}
			out_file.close();
#endif
			set_version( cache_msg.version() );

			// Check that this is our backpack.
#if !defined(READ_FROM_FILE)
			if (cache_msg.owner() != get_steam_id()) {
				return false;
			}
#endif

			// Notify that inventory is being reset.
            listener_->on_inventory_reset();
								
			// Add items.
            const unsigned int INVENTORY_ITEMS_SUBSCRIPTION_ID = 1;
            const unsigned int INVENTORY_SIZE_SUBSCRIPTION_ID = 7;
			for (int i = 0; i < cache_msg.objects_size(); i++) {
				CMsgSOCacheSubscribed_SubscribedType subscribed_type = cache_msg.objects( i );
				switch (subscribed_type.type_id()) {
				case INVENTORY_ITEMS_SUBSCRIPTION_ID:
					{
						// Ensure we own this item.
						for (int i = 0; i < subscribed_type.object_data_size(); i++) {
                            // Get data.
                            const char* data = subscribed_type.object_data( i ).data();
                            size_t size = subscribed_type.object_data( i ).size();
                            CSOEconItem econ_item;
							if (!econ_item.ParseFromArray( data, size )) {
                                stack->log( "Failed to parse item object from Steam message." );
                                return false;
                            }

                            // Get item from protobuf.
                            Item* item = create_item_from_message( &econ_item );
                            if (item == nullptr) {
                                stack->log( "Failed to create item from Steam object." );
                                return false;
                            }

                            // Send to listener.
                            if (!listener_->on_item_created( item )) {
                                JUTIL::BaseAllocator::destroy( item );
                                stack->log( "Failed to add item to inventory." );
                                return false;
                            }
						}

						break;
					}
				case INVENTORY_SIZE_SUBSCRIPTION_ID:
					{
                        // Ensure we own this inventory.
#if !defined(READ_FROM_FILE)
						if (get_steam_id() != cache_msg.owner()) {
							break;
						}
#endif
						for (int i = 0; i < subscribed_type.object_data_size(); i++) {
							CSOEconGameAccountClient client;
                            const char* data = subscribed_type.object_data( i ).data();
                            size_t size = subscribed_type.object_data( i ).size();
							if (!client.ParseFromArray( data, size )) {
                                stack->log( "Failed to parse account client object from Steam message." );
                                return false;
                            }

							// Check how many slots this backpack is supposed to have.
                            bool is_trial_account = client.trial_account();
							unsigned int extra_slots = client.additional_backpack_slots();
							if (!listener_->on_inventory_resize( is_trial_account, extra_slots )) {
                                stack->log( "Failed to resize inventory." );
                                return false;
                            }
						}

						break;
					}
				}
			}

            // Trigger post-load event.
            if (!listener_->on_inventory_loaded()) {
                stack->log( "Inventory post-load handling failed." );
                return false;
            }
			break;
		}

	case k_ESOMsg_CacheSubscriptionCheck:
		{
			CMsgSOCacheSubscriptionCheck check;
			if (!check.ParseFromArray( message, size )) {
                stack->log( "Failed to parse inventory subscription check." );
                return false;
            }

			// Compare version.
			uint64 version = check.version();
			if (get_version() != version) {
				set_version( version );

				// Send refresh.
				CMsgSOCacheSubscriptionRefresh refresh;
				refresh.set_owner( get_steam_id() );

                // Serialize and send.
				int size = refresh.ByteSize();
				unsigned int id = PROTOBUF_MESSAGE_FLAG | static_cast<unsigned int>(k_ESOMsg_CacheSubscriptionRefresh);
				JUTIL::ArrayBuilder<char> buffer;
				if (!buffer.set_size( size )) {
					stack->log( "Failed to size message buffer." );
					return false;
				}
				refresh.SerializeToArray( buffer.get_array(), size );
				if (!send_protobuf_message( job_id_source, id, buffer.get_array(), size )) {
                    stack->log( "Failed to send inventory subscription refresh to Steam." );
                    return false;
                }
			}
			break;
		}

	case k_ESOMsg_Update:
		{
			CMsgSOSingleObject update_msg;
			if (!update_msg.ParseFromArray( message, size )) {
                stack->log( "Failed to parse item update message from Steam." );
                return false;
            }
			set_version( update_msg.version() );

			if (update_msg.type_id() == 1) {
				CSOEconItem updated_item;
				if (!updated_item.ParseFromArray( update_msg.object_data().data(), update_msg.object_data().size() )) {
                    stack->log( "Failed to parse item update message from Steam." );
                    return false;
                }

                // Pass item ID and flags to listener.
                if (!listener_->on_item_updated( updated_item.id(), updated_item.inventory() )) {
                    stack->log( "Failed to update item." );
                    return false;
                }
			}

			break;
		}

	case k_ESOMsg_UpdateMultiple:
		{
			CMsgSOMultipleObjects update_msg;
			if (!update_msg.ParseFromArray( message, size )) {
                stack->log( "Failed to parse multiple item update message from Steam." );
                return false;
            }
			set_version( update_msg.version() );

			for (int i = 0; i < update_msg.objects_size(); i++) {
				CMsgSOMultipleObjects::SingleObject current_object = update_msg.objects( i );
				if (current_object.type_id() == 1) {
					CSOEconItem updated_item;
					if (!updated_item.ParseFromArray( current_object.object_data().data(), current_object.object_data().size() )) {
                        stack->log( "Failed to parse multiple item message from Steam." );
                        return false;
                    }

                    // Pass item ID and flags to listener.
                    if (!listener_->on_item_updated( updated_item.id(), updated_item.inventory() )) {
                        stack->log( "Failed to update item." );
                        return false;
                    }
				}
			}

			break;
		}

	case k_EMsgGCUpdateItemSchema:
		{
			// Not handling yet, since no translation.
			break;
		}

	case SOMsgCreate_t::k_iMessage:
		{
			// Get object created.
			CMsgSOSingleObject create_msg;
			if (!create_msg.ParseFromArray( message, size )) {
                stack->log( "Failed to parse item creation message from Steam." );
                return false;
            }
			set_version( create_msg.version() );

			// Get item from object.
			CSOEconItem created_item;
			if (!created_item.ParseFromArray( create_msg.object_data().data(), create_msg.object_data().size() )) {
                stack->log( "Failed to parse created item from Steam message." );
                return false;
            }

			// Now add item.
            Item* item = create_item_from_message( &created_item );
            if (item == nullptr) {
                stack->log( "Failed to create new item from Steam message." );
                return false;
            }

            // Pass new item to listener.
            if (!listener_->on_item_created( item )) {
                JUTIL::BaseAllocator::destroy( item );
                stack->log( "Failed to add item to inventory." );
                return false;
            }
			break;
		}

	case SOMsgDeleted_t::k_iMessage:
		{
			// Get deleted message.
			CMsgSOSingleObject delete_msg;
			if (!delete_msg.ParseFromArray( message, size )) {
                stack->log( "Failed to parse item deletion message from Steam." );
                return false;
            }
			set_version( delete_msg.version() );

			// Get ID of deleted item.
			CSOEconItem deleted_item;
			if (!deleted_item.ParseFromArray( delete_msg.object_data().data(), delete_msg.object_data().size() )) {
                stack->log( "Failed to parse deleted item object from Steam." );
                return false;
            }

            // Notify listener that this item is deleted.
            listener_->on_item_deleted( deleted_item.id() );
			break;
		}

	default:
		break;
	}

    return true;
}

/*
 * Generate an item from a JSON object.
 */
Item* SteamInventoryManager::create_item_from_message( CSOEconItem* econ_item )
{
	// Error stack for reporting.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Create item.
	Item* item;
	if (!JUTIL::BaseAllocator::allocate( &item )) {
		return nullptr;
	}
	item = new (item) Item(
		econ_item->id(),
		econ_item->def_index(),
		econ_item->level(),
		static_cast<EItemQuality>(econ_item->quality()),
		econ_item->quantity(),
		econ_item->inventory(),
		econ_item->flags(),
		econ_item->origin() );

	// Check equip fields.
	int j;
	if (econ_item->contains_equipped_state()) {
		for (j = 0; j < econ_item->equipped_state_size(); ++j) {
			const CSOEconItemEquipped& equipped = econ_item->equipped_state( j );
			EEquipClass equip_class = static_cast<EEquipClass>(equipped.new_class());
			EEquipSlot equip_slot = static_cast<EEquipSlot>(equipped.new_slot());
			if (equip_class != EQUIP_CLASS_SCOUT_UNUSED && equip_slot != EQUIP_SLOT_PREVIOUS) {
				if (!item->add_equipped_data( equip_class, equip_slot )) {
					JUTIL::BaseAllocator::destroy( item );
					stack->log( "Failed to add equipped status to item." );
					return nullptr;
				}
			}
		}
	}

	// Add the item's attributes.
	for (j = 0; j < econ_item->attribute_size(); ++j) {
		const CSOEconItemAttribute& attribute = econ_item->attribute( j );
		uint32 attrib_index = attribute.def_index();

		// Create attribute.
		const std::string& value_bytes = attribute.value_bytes();
		const char* value_buffer = value_bytes.c_str();
		size_t value_length = value_bytes.length();
		Attribute* new_attribute;
		if (!JUTIL::BaseAllocator::allocate( &new_attribute )) {
			JUTIL::BaseAllocator::destroy( item );
			stack->log( "Failed to allocate attribute for item." );
			return false;
		}
		new_attribute = new (new_attribute) Attribute( attrib_index );

		// Add to item and update value.
		if (!item->add_attribute( new_attribute )) {
			JUTIL::BaseAllocator::destroy( new_attribute );
			JUTIL::BaseAllocator::destroy( item );
			stack->log( "Failed to add attribute to item." );
			return false;
		}
		if (!new_attribute->set_value( value_buffer, value_length )) {
			JUTIL::BaseAllocator::destroy( item );
			stack->log( "Failed to set attribute value." );
			return false;
		}
	}

	return item;
}