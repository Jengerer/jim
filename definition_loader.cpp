#include "definition_loader.hpp"
#include <jui/net/file_downloader.hpp>

// Schema URL.
const JUTIL::ConstantString SCHEMA_URL = "http://api.steampowered.com/IEconItems_440/GetSchema/v0001/?key=0270F315C25E569307FEBDB67A497A2E&format=json&language=en";

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
        const JUTIL::ConstantString SLOTS_MAP_ERROR = "Failed to create slot map.";
        set_error( &SLOTS_MAP_ERROR );
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
        const JUTIL::ConstantString CLASSES_MAP_ERROR = "Failed to create class map.";
        set_error( &CLASSES_MAP_ERROR );
        return false;
    }

	set_state( LOADING_STATE_DOWNLOAD_DEFINITIONS );

	// Get downloader instance.
    JUI::FileDownloader* downloader = JUI::FileDownloader::get_instance();
    if (downloader == nullptr) {
        const JUTIL::ConstantString DOWNLOADER_ERROR = "Failed to create downloader.";
        set_error( &DOWNLOADER_ERROR );
        return false;
    }

	// Get definition file.
    JUTIL::DynamicString definition;
    if (!downloader->read( &SCHEMA_URL, &definition )) {
        const JUTIL::ConstantString READ_SCHEMA_ERROR = "Failed to read schema from Steam web API.";
        set_error( &READ_SCHEMA_ERROR );
        return false;
    }

	// Parse definition file.
	Json::Reader reader;
	if (!reader.parse( definition.get_string(), root_, false )) {
        const JUTIL::ConstantString PARSE_JSON_ERROR = "Failed to parse item definition JSON.";
        set_error( &PARSE_JSON_ERROR );
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
            const JUTIL::ConstantString ATTRIBUTE_READ_ERROR = "Unexpected attribute format found.";
            set_error( &ATTRIBUTE_READ_ERROR );
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
            const JUTIL::ConstantString UNEXPECTED_EFFECT_ERROR = "Unexpected attribute effect type received.";
            set_error( &UNEXPECTED_EFFECT_ERROR );
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
            const JUTIL::ConstantString ADD_ATTRIBUTE_ERROR = "Failed to add attribute to attribute map.";
            set_error( &ADD_ATTRIBUTE_ERROR );
            return false;
        }
        else if (!name_map_.insert( name_string, attrib_info )) {
            // Will be destroyed by item attributes, no need to delete.
            const JUTIL::ConstantString ADD_ATTRIBUTE_ERROR = "Failed to add attribute to attribute map.";
            set_error( &ADD_ATTRIBUTE_ERROR );
            return false;
        }

		loaded_attribs++;
		set_progress( loaded_attribs, num_attribs );
	}

	// Get item member.
    Json::Value* items;
    if (!get_member( result, &ITEMS_NAME, &items )) {
        const JUTIL::ConstantString ITEMS_NOT_FOUND_ERROR = "Failed to find item object in definitions.";
        set_error( &ITEMS_NOT_FOUND_ERROR );
        return false;
    }

	// Create fallback definition.
    JUI::FileTexture* unknown_item;
    if (!graphics_->get_texture( &FALLBACK_ITEM_TEXTURE, &unknown_item )) {
        const JUTIL::ConstantString FALLBACK_TEXTURE_LOAD_ERROR = "Failed to load texture for fallback/unknown item.";
        set_error( &FALLBACK_TEXTURE_LOAD_ERROR );
	    return false;
    }

    // Create definition.
    JUTIL::ConstantString* unknown_name;
    if (!JUTIL::BaseAllocator::allocate( &unknown_name )) {
        const JUTIL::ConstantString FALLBACK_DEFINITION_CREATE_ERROR = "Failed to create fallback item definition.";
        set_error( &FALLBACK_DEFINITION_CREATE_ERROR );
        return false;
    }
    unknown_name = new (unknown_name) JUTIL::ConstantString( &FALLBACK_ITEM_NAME );
    if (!JUTIL::BaseAllocator::allocate( &Item::fallback )) {
        JUTIL::BaseAllocator::destroy( unknown_name );
        const JUTIL::ConstantString FALLBACK_DEFINITION_CREATE_ERROR = "Failed to create fallback item definition.";
        set_error( &FALLBACK_DEFINITION_CREATE_ERROR );
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
    bool success = true;
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

bool DefinitionLoader::load_item( Json::Value* item,
    JUTIL::DynamicString* name,
    JUTIL::DynamicString* image,
    JUTIL::DynamicString* image_url )
{
    // Get all necessary attributes.
    Json::Value* item_index;
    Json::Value* item_name;
    if (!get_member( item, &ITEM_INDEX, &item_index ) || !get_member( item, &ITEM_NAME, &item_name )) {
        const JUTIL::ConstantString ITEM_FORMAT_ERROR = "Improper format for item in definitions.";
        set_error( &ITEM_FORMAT_ERROR );
        return false;
    }

    // Write name to string.
    if (!name->write( "%s", item_name->asCString() )) {
        JUTIL::BaseAllocator::destroy( name );
        const JUTIL::ConstantString ITEM_NAME_ERROR = "Failed to write item name string.";
        set_error( &ITEM_NAME_ERROR );
        return false;
    }

    // Write image and URL if reported by JSON; fallback if none.
    Json::Value* item_image;
    Json::Value* item_image_url;
    const JUTIL::ConstantString IMAGE_STRING_ERROR = "Failed to create image/URL string.";
    if (get_member( item, &ITEM_IMAGE, &item_image ) && get_member( item, &ITEM_IMAGE_URL, &item_image_url)) {
        // Write image file.
        const char* image_cstring = item_image->asCString();
        const char* image_url_cstring = item_image_url->asCString();
        if (!image->write( "img/%s.png", image_cstring ) || !image_url->write( image_url_cstring )) {
            set_error( &IMAGE_STRING_ERROR );
            return false;
        }
    }
    else {
        // Copy fallback.
        if (!image->copy( &FALLBACK_ITEM_TEXTURE ) || !image_url->copy( &FALLBACK_ITEM_TEXTURE_URL )) {
            set_error( &IMAGE_STRING_ERROR );
            return false;
        }
    }

    // Load item slots.
    Json::Value* item_slot;
    EItemSlot slot = SLOT_NONE;
    if (get_member( item, &ITEM_SLOT, &item_slot )) {
        const JUTIL::ConstantString slot_name = item_slot->asCString();
        if (!slots_.get( &slot_name, &slot )) {
            const JUTIL::ConstantString UNKNOWN_SLOT_ERROR = "Unknown slot type found in definitions.";
            set_error( &UNKNOWN_SLOT_ERROR );
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
                const JUTIL::ConstantString UNKNOWN_CLASS_ERROR = "Unknown class type found in definitions.";
                set_error( &UNKNOWN_CLASS_ERROR );
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
        const JUTIL::ConstantString INFORMATION_ALLOCATE_ERROR = "Failed to create item information object.";
        set_error( &INFORMATION_ALLOCATE_ERROR );
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
                const JUTIL::ConstantString ATTRIBUTE_LOAD_ERROR = "Failed to load attribute for item.";
                set_error( &ATTRIBUTE_LOAD_ERROR );
                return false;
            }
		}
    }

	// Parse item type and insert.
    uint16 index = static_cast<uint16>(item_index->asUInt());
    if (!Item::definitions.insert( index, information )) {
        JUTIL::BaseAllocator::destroy( information );
        const JUTIL::ConstantString ITEM_INFORMATION_ERROR = "Failed to add item information to map.";
        set_error( &ITEM_INFORMATION_ERROR );
        return false;
    }

    // All successfully added! Phew!
    return true;
}

bool DefinitionLoader::load_item_attribute( Json::Value* attribute, ItemInformation* information )
{
    // Get attribute name.
    Json::Value* attribute_name;
    Json::Value* attribute_value;
    if (!get_member( attribute, &ATTRIBUTE_NAME, &attribute_name )) {
        const JUTIL::ConstantString ATTRIBUTE_NAME_ERROR = "Failed to get name from attribute definition.";
        set_error( &ATTRIBUTE_NAME_ERROR );
        return false;
    }
    else if (!get_member( attribute, &ATTRIBUTE_VALUE, &attribute_value )) {
        const JUTIL::ConstantString ATTRIBUTE_VALUE_ERROR = "Failed to get value from attribute definition.";
        set_error( &ATTRIBUTE_VALUE_ERROR );
        return false;
    }
    const JUTIL::ConstantString name = attribute_name->asCString();
	float value = attribute_value->asFloat();

	// Get information.
    AttributeInformation* attribute_information;
    if (!name_map_.get( &name, &attribute_information )) {
        const JUTIL::ConstantString ATTRIBUTE_NOT_FOUND_ERROR = "Failed to find attribute by name.";
        set_error( &ATTRIBUTE_NOT_FOUND_ERROR );
        return false;
	}

	// Create new attribute.
    Attribute* new_attribute;
    if (!JUTIL::BaseAllocator::allocate( &new_attribute )) {
        const JUTIL::ConstantString ATTRIBUTE_ALLOCATE_ERROR = "Failed to create attribute.";
        set_error( &ATTRIBUTE_ALLOCATE_ERROR );
        return false;
    }
    new_attribute = new (new_attribute) Attribute( attribute_information, value );
    if (information->add_attribute( new_attribute )) {
        const JUTIL::ConstantString ATTRIBUTE_ADD_ERROR = "Failed to add attribute to item definition.";
        set_error( &ATTRIBUTE_ADD_ERROR );
        return false;
    }

    // Finished!
    return true;
}

void DefinitionLoader::clean_up()
{
	set_state( LOADING_STATE_CLEANUP );
	root_.clear();
	slots_.clear();
	classes_.clear();
	graphics_->set_render_context( graphics_->get_loading_context() );
}

void DefinitionLoader::set_state( ELoadingState state )
{
	state_ = state;
	set_progress( 0.0f );
	state_changed_ = true;
	update_progress_msg();
}

ELoadingState DefinitionLoader::get_state() const
{
	return state_;
}

float DefinitionLoader::get_progress() const
{
	return progress_ * 100.0f;
}

void DefinitionLoader::set_error( const std::string& error_msg )
{
	error_msg_ = error_msg;
	set_state( LOADING_STATE_ERROR );
}

void DefinitionLoader::set_progress( float percentage )
{
	progress_ = percentage;
}

void DefinitionLoader::set_progress( size_t loaded, size_t total )
{
	set_progress( static_cast<float>(loaded) / static_cast<float>(total) );
}

void DefinitionLoader::set_progress_msg( const std::string& progress_msg )
{
	progress_msg_ = progress_msg;
}

const std::string& DefinitionLoader::get_progress_msg() const
{
	return progress_msg_;
}

void DefinitionLoader::update_progress_msg()
{
	std::stringstream message;
	message.precision( 3 );

	switch (get_state()) {
	case LOADING_STATE_NONE:
		message << "Definition loader not started.";
		break;

	case LOADING_STATE_START:
		message << "Starting definition loader...";
		break;

	case LOADING_STATE_DOWNLOAD_DEFINITIONS:
		message << "Downloading item schema from Steam Web API...";
		break;

	case LOADING_STATE_LOADING_ATTRIBUTES:
		message << "Loading attributes from schema...\n(" << get_progress() << "%)";
		break;

	case LOADING_STATE_LOADING_ITEMS:
		message << "Loading items from schema...\n(" << get_progress() << "%)";
		break;

	case LOADING_STATE_ERROR:
		message << error_msg_;
		break;

	case LOADING_STATE_CLEANUP:
		message << "Cleaning up...";
		break;

	case LOADING_STATE_FINISHED:
		message << "Loading item definitions completed successfully.";
		break;
	}

	set_progress_msg( message.str() );
}