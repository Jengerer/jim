#include "definition_loader.hpp"
#include <jui/application/error_stack.hpp>
#include <jui/net/file_downloader.hpp>

// Schema URL.
const JUTIL::ConstantString SCHEMA_URL = "http://api.steampowered.com/IEconItems_440/GetSchema/v0001/?key=0270F315C25E569307FEBDB67A497A2E&format=json&language=en";

// Status strings.
const JUTIL::ConstantString DEFAULT_STATUS_MESSAGE = "Loading...\nCouldn't generate progress string...\nJengerer's prescription: proceed as normal...";

// Slot name strings.
const JUTIL::ConstantString SLOT_NONE_NAME = "";
const JUTIL::ConstantString SLOT_INVALID_NAME = "invalid";
const JUTIL::ConstantString SLOT_PRIMARY_NAME = "primary";
const JUTIL::ConstantString SLOT_SECONDARY_NAME = "secondary";
const JUTIL::ConstantString SLOT_MELEE_NAME = "melee";
const JUTIL::ConstantString SLOT_PDA_NAME = "pda";
const JUTIL::ConstantString SLOT_PDA2_NAME = "pda2";
const JUTIL::ConstantString SLOT_BUILDING_NAME = "building";
const JUTIL::ConstantString SLOT_HEAD_NAME = "head";
const JUTIL::ConstantString SLOT_MISC_NAME = "misc";
const JUTIL::ConstantString SLOT_ACTION_NAME = "action";
const JUTIL::ConstantString SLOT_GRENADE_NAME = "grenade";

// Class name strings.
const JUTIL::ConstantString CLASS_NONE_NAME = "";
const JUTIL::ConstantString CLASS_SCOUT_NAME = "Scout";
const JUTIL::ConstantString CLASS_SOLDIER_NAME = "Soldier";
const JUTIL::ConstantString CLASS_PYRO_NAME = "Pyro";
const JUTIL::ConstantString CLASS_DEMOMAN_NAME = "Demoman";
const JUTIL::ConstantString CLASS_HEAVY_NAME = "Heavy";
const JUTIL::ConstantString CLASS_ENGINEER_NAME = "Engineer";
const JUTIL::ConstantString CLASS_MEDIC_NAME = "Medic";
const JUTIL::ConstantString CLASS_SNIPER_NAME = "Sniper";
const JUTIL::ConstantString CLASS_SPY_NAME = "Spy";

// Attribute effect names.
const JUTIL::ConstantString EFFECT_TYPE_POSITIVE = "positive";
const JUTIL::ConstantString EFFECT_TYPE_NEGATIVE = "negative";
const JUTIL::ConstantString EFFECT_TYPE_NEUTRAL = "neutral";

// Fallback definitions.
const JUTIL::ConstantString FALLBACK_ITEM_TEXTURE = "img/backpack/unknown_item.png";
const JUTIL::ConstantString FALLBACK_ITEM_TEXTURE_URL = "http://www.jengerer.com/item_manager/img/backpack/unknown_item.png";
const JUTIL::ConstantString FALLBACK_ITEM_NAME = "Unknown Item";
const unsigned int FALLBACK_ITEM_CLASS_FLAGS = 0;
const EItemSlot FALLBACK_ITEM_SLOT = SLOT_NONE;

// JSON parsing object names.
const JUTIL::ConstantString RESULT_NAME = "result";
const JUTIL::ConstantString ATTRIBUTES_NAME = "attributes";
const JUTIL::ConstantString ITEMS_NAME = "items";

// Attribute JSON members.
const JUTIL::ConstantString ATTRIBUTE_NAME = "name";
const JUTIL::ConstantString ATTRIBUTE_INDEX = "defindex";
const JUTIL::ConstantString ATTRIBUTE_CLASS = "attribute_class";
const JUTIL::ConstantString ATTRIBUTE_VALUE = "value";
const JUTIL::ConstantString ATTRIBUTE_MIN_VALUE = "min_value";
const JUTIL::ConstantString ATTRIBUTE_MAX_VALUE = "max_value";
const JUTIL::ConstantString ATTRIBUTE_EFFECT_TYPE = "effect_type";
const JUTIL::ConstantString ATTRIBUTE_IS_HIDDEN = "hidden";
const JUTIL::ConstantString ATTRIBUTE_IS_INTEGER = "stored_as_integer";
const JUTIL::ConstantString ATTRIBUTE_DESCRIPTION = "description_string";
const JUTIL::ConstantString ATTRIBUTE_DESCRIPTION_FORMAT = "description_format";

// Item JSON members.
const JUTIL::ConstantString ITEM_INDEX = "defindex";
const JUTIL::ConstantString ITEM_NAME = "item_name";
const JUTIL::ConstantString ITEM_IMAGE = "image_inventory";
const JUTIL::ConstantString ITEM_IMAGE_URL = "image_url";
const JUTIL::ConstantString ITEM_SLOT = "item_slot";
const JUTIL::ConstantString ITEM_CLASSES = "used_by_classes";
const JUTIL::ConstantString ITEM_ATTRIBUTES = "attributes";

/*
 * Get the member from a node if it exists.
 */
bool get_member( Json::Value* root, const JUTIL::String* member, Json::Value** output )
{
    // Verify that member exists.
    if (!root->isMember( member->get_string() )) {
        return false;
    }

    *output = &(*root)[ member->get_string() ];
    return true;
}

/*
 * Definition loader constructor by graphics object.
 */
DefinitionLoader::DefinitionLoader( JUI::Graphics2D* graphics )
{
	graphics_ = graphics;

    // Initialize progress/state.
	set_progress( 0.0f );
	set_state( LOADING_STATE_NONE );
}

/*
 * Definition loader destructor.
 */
DefinitionLoader::~DefinitionLoader( void )
{
	if (thread_ != nullptr) {
		end();
	}
}

/*
 * Attempt to start the loader.
 */
bool DefinitionLoader::begin( void )
{
    // Shouldn't begin if thread created.
	assert( thread_ == nullptr );
	should_stop_ = false;
	set_state( LOADING_STATE_START );

    // Create worker thread.
    if (!JUTIL::BaseAllocator::allocate( &thread_ )) {
        return false;
    }
	thread_ = new (thread_) boost::thread( boost::bind( &DefinitionLoader::load, this ) );
    return true;
}

/*
 * Finish loading definitions.
 */
void DefinitionLoader::end( void )
{
	assert( thread_ != nullptr );

    // Indicate end and join.
	should_stop_ = true;
	thread_->join();
}

/*
 * Load item definitions.
 */
bool DefinitionLoader::load()
{
	// Get error stack.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Set this thread's render context.
	graphics_->set_render_context( graphics_->get_loading_context() );

	// Create slot name map.
    bool success = slots_.insert( &SLOT_NONE_NAME, SLOT_NONE ) &&
        slots_.insert( &SLOT_INVALID_NAME, SLOT_INVALID ) &&
        slots_.insert( &SLOT_PRIMARY_NAME, SLOT_PRIMARY ) &&
        slots_.insert( &SLOT_SECONDARY_NAME, SLOT_SECONDARY ) &&
        slots_.insert( &SLOT_MELEE_NAME, SLOT_MELEE ) &&
        slots_.insert( &SLOT_PDA_NAME, SLOT_PDA ) &&
        slots_.insert( &SLOT_PDA2_NAME, SLOT_PDA2 ) &&
        slots_.insert( &SLOT_BUILDING_NAME, SLOT_BUILDING ) &&
        slots_.insert( &SLOT_HEAD_NAME, SLOT_HEAD ) &&
        slots_.insert( &SLOT_MISC_NAME, SLOT_MISC ) &&
        slots_.insert( &SLOT_ACTION_NAME, SLOT_ACTION ) &&
        slots_.insert( &SLOT_GRENADE_NAME, SLOT_GRENADE );
    if (!success) {
        stack->log( "Failed to create slot map." );
        return false;
    }

	// Create class map.
    success = classes_.insert( &CLASS_NONE_NAME, CLASS_NONE ) &&
        classes_.insert( &CLASS_SCOUT_NAME, CLASS_SCOUT ) &&
        classes_.insert( &CLASS_SOLDIER_NAME, CLASS_SOLDIER ) &&
        classes_.insert( &CLASS_PYRO_NAME, CLASS_PYRO ) &&
        classes_.insert( &CLASS_DEMOMAN_NAME, CLASS_DEMOMAN ) &&
        classes_.insert( &CLASS_HEAVY_NAME, CLASS_HEAVY ) &&
        classes_.insert( &CLASS_ENGINEER_NAME, CLASS_ENGINEER ) &&
        classes_.insert( &CLASS_MEDIC_NAME, CLASS_MEDIC ) &&
        classes_.insert( &CLASS_SNIPER_NAME, CLASS_SNIPER ) &&
        classes_.insert( &CLASS_SPY_NAME, CLASS_SPY );
    if (!success) {
        stack->log( "Failed to create class map.");
        return false;
    }

	set_state( LOADING_STATE_DOWNLOAD_DEFINITIONS );

	// Get downloader instance.
    JUI::FileDownloader* downloader = JUI::FileDownloader::get_instance();
    if (downloader == nullptr) {
        stack->log( "Failed to create downloader.");
        return false;
    }

	// Get definition file.
    JUTIL::DynamicString definition;
    if (!downloader->read( &SCHEMA_URL, &definition )) {
        stack->log( "Failed to read schema from Steam web API.");
        return false;
    }

	// Parse definition file.
	Json::Reader reader;
	if (!reader.parse( definition.get_string(), root_, false )) {
        stack->log( "Failed to parse item definition JSON.");
        return false;
	}

    // Get result.
    Json::Value* result;
    Json::Value* attributes;
    if (!get_member( &root_, &RESULT_NAME, &result ) || !get_member( &root_, &ATTRIBUTES_NAME, &attributes )) {
        return false;
    }
		
	// Start loading attributes.
	size_t loaded_attribs = 0;
	size_t num_attribs = attributes->size();
	set_state( LOADING_STATE_LOADING_ATTRIBUTES );
	for (Json::ValueIterator i = attributes->begin(); i != attributes->end(); ++i) {
		// Quit if we need to stop.
		if (should_stop_) {
			return false;
		}

		Json::Value& attribute = *i;

		// Get necessary members.
        Json::Value* attribute_name;
        Json::Value* attribute_index;
        Json::Value* attribute_class;
        Json::Value* attribute_min_value;
        Json::Value* attribute_max_value;
        Json::Value* attribute_effect_type;
        Json::Value* attribute_is_hidden;
        Json::Value* attribute_is_integer;
        bool success = get_member( &attribute, &ATTRIBUTE_NAME, &attribute_name ) &&
            get_member( &attribute, &ATTRIBUTE_INDEX, &attribute_index ) &&
            get_member( &attribute, &ATTRIBUTE_CLASS, &attribute_class ) &&
            get_member( &attribute, &ATTRIBUTE_MIN_VALUE, &attribute_min_value ) &&
            get_member( &attribute, &ATTRIBUTE_MAX_VALUE, &attribute_max_value ) &&
            get_member( &attribute, &ATTRIBUTE_EFFECT_TYPE, &attribute_effect_type ) &&
            get_member( &attribute, &ATTRIBUTE_IS_HIDDEN, &attribute_is_hidden ) &&
            get_member( &attribute, &ATTRIBUTE_IS_INTEGER, &attribute_is_integer );
        if (!success) {
            stack->log( "Unexpected attribute format found." );
            return false;
        }

		// Get effect type, third char is different for each time.
        const JUTIL::ConstantString effect_string = attribute_effect_type->asCString();
		EffectType effect_type;
		if (effect_string.is_equal( &EFFECT_TYPE_POSITIVE )) {
			effect_type = EFFECT_POSITIVE;
		}
		else if (effect_string.is_equal( &EFFECT_TYPE_NEGATIVE )) {
			effect_type = EFFECT_NEGATIVE;
		}
        else if (effect_string.is_equal( &EFFECT_TYPE_NEUTRAL )) {
			effect_type = EFFECT_NEUTRAL;
		}
		else {
            stack->log( "Unexpected attribute effect type received.");
            return false;
		}

		// Allocate new name string.
        JUTIL::DynamicString* name_string;
        if (!JUTIL::BaseAllocator::allocate( &name_string )) {
            return false;
        }
        name_string = new (name_string) JUTIL::DynamicString();
        if (!name_string->write( "%s", attribute_name->asCString() )) {
            JUTIL::BaseAllocator::destroy( name_string );
            return false;
        }

        // Allocate class string.
        JUTIL::DynamicString* class_string;
        if (!JUTIL::BaseAllocator::allocate( &class_string )) {
            JUTIL::BaseAllocator::destroy( name_string );
            return false;
        }
        class_string = new (class_string) JUTIL::DynamicString();
        if (!class_string->write( "%s", attribute_class->asCString() )) {
            JUTIL::BaseAllocator::destroy( name_string );
            JUTIL::BaseAllocator::destroy( class_string );
            return false;
        }

        // Get rest of parameters and create information object.
        unsigned int index = attribute_index->asUInt();
        float min_value = attribute_min_value->asFloat();
        float max_value = attribute_max_value->asFloat();
        bool is_hidden = attribute_is_hidden->asBool();
        bool is_integer = attribute_is_integer->asBool();

        // Allocate information object.
        AttributeInformation* attrib_info;
        if (!JUTIL::BaseAllocator::allocate( &attrib_info )) {
            JUTIL::BaseAllocator::destroy( name_string );
            JUTIL::BaseAllocator::destroy( class_string );
            return false;
        }
		attrib_info = new (attrib_info) AttributeInformation(
			name_string,
			class_string,
            index,
			min_value, max_value,
			effect_type,
			is_hidden,
			is_integer );

		// Check for optional members.
        Json::Value* attribute_description;
        Json::Value* attribute_format;
        bool has_description = get_member( &attribute, &ATTRIBUTE_DESCRIPTION, &attribute_description ) &&
            get_member( &attribute, &ATTRIBUTE_DESCRIPTION_FORMAT, &attribute_format );
        if (has_description) {
            JUTIL::DynamicString* description;
            JUTIL::DynamicString* format;

            // Allocate and write to strings.
            if (!JUTIL::BaseAllocator::allocate( &description )) {
                JUTIL::BaseAllocator::destroy( attrib_info );
                return false;
            }
            description = new (description) JUTIL::DynamicString();
            if (!JUTIL::BaseAllocator::allocate( &format )) {
                JUTIL::BaseAllocator::destroy( description );
                JUTIL::BaseAllocator::destroy( attrib_info );
            }
            format = new (format) JUTIL::DynamicString();

            // Set description and update.
            attrib_info->set_description( description, format );

            bool success = description->write( "%s", attribute_description->asCString() ) &&
                format->write( "%s", attribute_format->asCString() );
            if (!success) {
                JUTIL::BaseAllocator::destroy( attrib_info );
                return false;
            }
        }
        
        // Add to maps.
        if (!Item::attributes.insert( index, attrib_info )) {
            JUTIL::BaseAllocator::destroy( attrib_info );
            stack->log( "Failed to add attribute to attribute map.");
            return false;
        }
        else if (!name_map_.insert( name_string, attrib_info )) {
            // Will be destroyed by item attributes, no need to delete.
            stack->log( "Failed to add attribute to attribute map.");
            return false;
        }

		loaded_attribs++;
		set_progress( loaded_attribs, num_attribs );
	}

	// Get item member.
    Json::Value* items;
    if (!get_member( result, &ITEMS_NAME, &items )) {
        stack->log( "Failed to find item object in definitions.");
        return false;
    }

	// Create fallback definition.
    JUI::FileTexture* unknown_item;
    if (!graphics_->get_texture( &FALLBACK_ITEM_TEXTURE, &unknown_item )) {
        stack->log( "Failed to load texture for fallback/unknown item.");
	    return false;
    }

    // Create definition.
    JUTIL::ConstantString* unknown_name;
    if (!JUTIL::BaseAllocator::allocate( &unknown_name )) {
        stack->log( "Failed to create fallback item definition.");
        return false;
    }
    unknown_name = new (unknown_name) JUTIL::ConstantString( &FALLBACK_ITEM_NAME );
    if (!JUTIL::BaseAllocator::allocate( &Item::fallback )) {
        JUTIL::BaseAllocator::destroy( unknown_name );
        stack->log( "Failed to create fallback item definition.");
        return false;
    }
	Item::fallback = new (Item::fallback) ItemInformation(
		unknown_name,
		unknown_item,
		FALLBACK_ITEM_CLASS_FLAGS,
		FALLBACK_ITEM_SLOT );

    // Strings for parsing.
    JUTIL::DynamicString* name = nullptr;
    JUTIL::DynamicString* image = nullptr;
    JUTIL::DynamicString* image_url = nullptr;

	// Start loading items.
	success = true;
	size_t loaded_items = 0;
	size_t num_items = items->size();
	set_state( LOADING_STATE_LOADING_ITEMS );
	for (Json::ValueIterator i = items->begin(); i != items->end(); ++i) {
		// Return if we're exiting.
		if (should_stop_) {
			success = false;
            break;
		}

        // Allocate strings for parsing.
        if (!JUTIL::BaseAllocator::allocate( &name )) {
            success = false;
            break;
        }
        name = new (name) JUTIL::DynamicString();
        if (!JUTIL::BaseAllocator::allocate( &image )) {
            success = false;
            break;
        }
        image = new (image) JUTIL::DynamicString();
        if (!JUTIL::BaseAllocator::allocate( &image_url )) {
            success = false;
            break;
        }
        image_url = new (image_url) JUTIL::DynamicString();

        // Load item.
		Json::Value* item = &(*i);
        if (!load_item( item, name, image, image_url )) {
            success = false;
            JUTIL::BaseAllocator::destroy( name );
            JUTIL::BaseAllocator::destroy( image );
            JUTIL::BaseAllocator::destroy( image_url );
            break;
        }

		// Update progress.
		loaded_items++;
		set_progress( loaded_items, num_items );
	}

	clean_up();
	set_state( LOADING_STATE_FINISHED );
}

/*
 * Load an item from a JSON object.
 * Returns true if item loaded successfully, false otherwise.
 *
 * Parameters:
 * name - Dynamic string to fill out with item's name.
 * image - Dynamic string to fill with item's image file name.
 * image_url - Dynamic string to fill with item's image download URL.
 */
bool DefinitionLoader::load_item( Json::Value* item,
    JUTIL::DynamicString* name,
    JUTIL::DynamicString* image,
    JUTIL::DynamicString* image_url )
{
	// Stack for error reporting.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Get all necessary attributes.
    Json::Value* item_index;
    Json::Value* item_name;
    if (!get_member( item, &ITEM_INDEX, &item_index ) || !get_member( item, &ITEM_NAME, &item_name )) {
        stack->log( "Improper format for item in definitions.");
        return false;
    }

    // Write name to string.
    if (!name->write( "%s", item_name->asCString() )) {
        JUTIL::BaseAllocator::destroy( name );
        stack->log( "Failed to write item name string.");
        return false;
    }

    // Write image and URL if reported by JSON; fallback if none.
    Json::Value* item_image;
    Json::Value* item_image_url;
    if (get_member( item, &ITEM_IMAGE, &item_image ) && get_member( item, &ITEM_IMAGE_URL, &item_image_url)) {
        // Write image file.
        const char* image_cstring = item_image->asCString();
        const char* image_url_cstring = item_image_url->asCString();
        if (!image->write( "img/%s.png", image_cstring )) {
			stack->log( "Failed to write image path for item definition." );
			return false;
		}
		else if (!image_url->write( image_url_cstring )) {
            stack->log( "Failed to create image or URL string for item definition." );
            return false;
        }
    }
    else {
        // Copy fallback.
        if (!image->copy( &FALLBACK_ITEM_TEXTURE )) {
			stack->log( "Failed to copy fallback item texture name." );
            return false;
        }
		else if ( !image_url->copy( &FALLBACK_ITEM_TEXTURE_URL )) {
			stack->log( "Failed to copy fallback item texture URL." );
			return false;
		}
    }

    // Load item slots.
    Json::Value* item_slot;
    EItemSlot slot = SLOT_NONE;
    if (get_member( item, &ITEM_SLOT, &item_slot )) {
        const JUTIL::ConstantString slot_name = item_slot->asCString();
        if (!slots_.get( &slot_name, &slot )) {
            stack->log( "Unknown slot type found in definitions.");
            return false;
        }
    }

	// Get classes, if they exist.
    Json::Value* item_classes;
	unsigned int classes = CLASS_ALL;
    if (get_member( item, &ITEM_CLASSES, &item_classes )) {
        size_t i;
        size_t length = item_classes->size();
        for (i = 0; i < length; ++i) {
            const JUTIL::ConstantString class_name = (*item_classes)[i].asCString();

            // Find enum for name.
            EClassEquip class_equip;
            if (!classes_.get( &class_name, &class_equip )) {
                stack->log( "Unknown class type found in definitions.");
                return false;
            }

            classes |= static_cast<unsigned int>(class_equip);
        }
    }

	// Check that texture exists.
	const JUI::Texture* texture = nullptr;
	JUI::FileDownloader* downloader = JUI::FileDownloader::get_instance();

    // Load texture; fall back to default if failed.
    JUI::FileTexture* item_texture;
    if (downloader->check_and_get( image, image_url ) && graphics_->get_texture( image, &item_texture )) {
        texture = static_cast<JUI::Texture*>(item_texture);
    }
    else {
        texture = Item::fallback->get_texture();
    }

	// Generate information object.
    ItemInformation* information;
    if (!JUTIL::BaseAllocator::allocate( &information )) {
        stack->log( "Failed to create item information object.");
        return false;
    }
    information = new (information) ItemInformation(
        name,
        texture,
        classes,
        slot );

	// Now add attributes, if they exist.
    Json::Value* item_attributes;
    if (get_member( item, &ITEM_ATTRIBUTES, &item_attributes )) {
        // Iterate through all attributes.
		for (Json::ValueIterator j = item_attributes->begin(); j != item_attributes->end(); ++j) {
			Json::Value* attribute = &(*j);

			// Get class and value.
            if (!load_item_attribute( attribute, information )) {
                stack->log( "Failed to load attribute for item.");
                return false;
            }
		}
    }

	// Parse item type and insert.
    uint16 index = static_cast<uint16>(item_index->asUInt());
    if (!Item::definitions.insert( index, information )) {
        JUTIL::BaseAllocator::destroy( information );
        stack->log( "Failed to add item information to map.");
        return false;
    }

    // All successfully added! Phew!
    return true;
}

/*
 * Load an item attribute from JSON object.
 * Returns true on success, false otherwise.
 *
 * Parameters:
 * information - Item information to add attribute to on success.
 */
bool DefinitionLoader::load_item_attribute( Json::Value* attribute, ItemInformation* information )
{
	// Get error stack for reporting failure.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Get attribute name.
    Json::Value* attribute_name;
    Json::Value* attribute_value;
    if (!get_member( attribute, &ATTRIBUTE_NAME, &attribute_name )) {
        stack->log( "Failed to get name from attribute definition.");
        return false;
    }
    else if (!get_member( attribute, &ATTRIBUTE_VALUE, &attribute_value )) {
        stack->log( "Failed to get value from attribute definition.");
        return false;
    }
    const JUTIL::ConstantString name = attribute_name->asCString();
	float value = attribute_value->asFloat();

	// Get information.
    AttributeInformation* attribute_information;
    if (!name_map_.get( &name, &attribute_information )) {
        stack->log( "Failed to find attribute by name.");
        return false;
	}

	// Create new attribute.
    Attribute* new_attribute;
    if (!JUTIL::BaseAllocator::allocate( &new_attribute )) {
        stack->log( "Failed to create attribute.");
        return false;
    }
    new_attribute = new (new_attribute) Attribute( attribute_information, value );
    if (information->add_attribute( new_attribute )) {
        stack->log( "Failed to add attribute to item definition.");
        return false;
    }

    // Finished!
    return true;
}

/*
 * Clean up definition loader.
 */
void DefinitionLoader::clean_up( void )
{
	set_state( LOADING_STATE_CLEANUP );
	root_.clear();
	slots_.clear();
	classes_.clear();
	graphics_->set_render_context( graphics_->get_loading_context() );
}

/*
 * Set definition loader state.
 */
void DefinitionLoader::set_state( DefinitionLoaderState state )
{
	state_ = state;
	set_progress( 0.0f );
	has_state_changed_ = true;
}

/*
 * Get definition loading state.
 */
DefinitionLoaderState DefinitionLoader::get_state( void ) const
{
	return state_;
}

/*
 * Get progress as floating point percentage.
 */
float DefinitionLoader::get_progress( void ) const
{
	return progress_ * 100.0f;
}

/*
 * Set progress as floating point percentage.
 */
void DefinitionLoader::set_progress( float percentage )
{
	if (progress_ != percentage) {
		progress_ = percentage;
		has_state_changed_ = true;
	}
}

/*
 * Set progress by current and total.
 */
void DefinitionLoader::set_progress( size_t loaded, size_t total )
{
	float percent = static_cast<float>(loaded) / static_cast<float>(total);
	set_progress( percent );
}

/*
 * Get progress message if any.
 */
const JUTIL::String* DefinitionLoader::get_progress_message( void )
{
	// Update progress message if state changed.
	if (has_state_changed_) {
		has_state_changed_ = false;
		if (!update_progress_message()) {
			return &DEFAULT_STATUS_MESSAGE;
		}
	}

	return &progress_message_;
}

/*
 * Update progress message with new information.
 */
bool DefinitionLoader::update_progress_message( void )
{
	// Erase old message.
	progress_message_.clear();

	// Write new state.
	float progress = get_progress();
	switch (get_state()) {
	case LOADING_STATE_NONE:
		if (!progress_message_.write( "Definition loader not started." )) {
			return false;
		}
		break;

	case LOADING_STATE_START:
		if (!progress_message_.write( "Starting definition loader..." )) {
			return false;
		}
		break;

	case LOADING_STATE_DOWNLOAD_DEFINITIONS:
		if (!progress_message_.write( "Downloading item schema from Steam Web API..." )) {
			return false;
		}
		break;

	case LOADING_STATE_LOADING_ATTRIBUTES:
		if (!progress_message_.write( "Loading attributes from schema...\n%.2f\%", progress )) {
			return false;
		}
		break;

	case LOADING_STATE_LOADING_ITEMS:
		if (!progress_message_.write( "Loading items from schema...\n%.2f\%", progress )) {
			return false;
		}
		break;

	case LOADING_STATE_ERROR:
		if (!progress_message_.write( "Error occurred, exiting definition loader..." )) {
			return false;
		}
		break;

	case LOADING_STATE_CLEANUP:
		if (!progress_message_.write( "Cleaning up..." )) {
			return false;
		}
		break;

	case LOADING_STATE_FINISHED:
		if (!progress_message_.write( "Loading item definitions completed successfully." )) {
			return false;
		}
		break;
	}

	// Successfully set.
	return true;
}