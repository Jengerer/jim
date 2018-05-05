#include "definition_loader.hpp"
#include <jui/application/error_stack.hpp>
#include <jui/net/file_downloader.hpp>
#include <cassert>

// Schema URL.
const JUTIL::ConstantString SCHEMA_ITEMS_URL = "http://api.steampowered.com/IEconItems_440/GetSchemaItems/v0001/?key=0270F315C25E569307FEBDB67A497A2E&format=json&language=en";
const JUTIL::ConstantString SCHEMA_OVERVIEW_URL = "http://api.steampowered.com/IEconItems_440/GetSchemaOverview/v0001/?key=0270F315C25E569307FEBDB67A497A2E&format=json&language=en";
const JUTIL::ConstantString SCHEMA_ITEMS_FILE_LOCATION = "schema/items.json";
const JUTIL::ConstantString SCHEMA_OVERVIEW_FILE_LOCATION = "schema/overview.json";
const JUTIL::ConstantString SCHEMA_CACHED_LOAD_FAIL_MESSAGE = "Failed to load cached item schema!";

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
const JUTIL::ConstantString EFFECT_TYPE_VISUALS_MVM_BOSS = "visuals_mvm_boss";
const JUTIL::ConstantString EFFECT_TYPE_FROM_LUT = "value_is_from_lookup_table";

// Fallback definitions.
const JUTIL::ConstantString FALLBACK_ITEM_TEXTURE = "img/backpack/unknown_item.png";
const JUTIL::ConstantString FALLBACK_ITEM_TEXTURE_URL = "http://www.jengerer.com/item_manager/img/backpack/unknown_item.png";
const JUTIL::ConstantString FALLBACK_ITEM_NAME = "Unknown Item";
const unsigned int FALLBACK_ITEM_CLASS_FLAGS = 0;
const EItemSlot FALLBACK_ITEM_SLOT = ITEM_SLOT_NONE;

// JSON parsing object names.
const JUTIL::ConstantString RESULT_KEY = "result";
const JUTIL::ConstantString ATTRIBUTES_KEY = "attributes";
const JUTIL::ConstantString ITEMS_KEY = "items";
const JUTIL::ConstantString QUALITIES_KEY = "qualities";
const JUTIL::ConstantString QUALITY_NAMES_KEY = "qualityNames";
const JUTIL::ConstantString ORIGIN_NAMES_KEY = "originNames";
const JUTIL::ConstantString ORIGIN_INDEX_KEY = "origin";
const JUTIL::ConstantString ORIGIN_NAME_KEY = "name";
const JUTIL::ConstantString KILL_EATER_CLASS_KEY = "name";
const JUTIL::ConstantString TYPE_NAME = "type";
const JUTIL::ConstantString KILL_EATER_TYPE_KEY = "kill_eater_score_types";
const JUTIL::ConstantString KILL_EATER_TYPE_INDEX_KEY = "type";
const JUTIL::ConstantString KILL_EATER_TYPE_NAME_KEY = "type_name";
const JUTIL::ConstantString KILL_EATER_TYPE_LEVEL_DATA_KEY = "level_data";
const JUTIL::ConstantString KILL_EATER_RANKS_KEY = "item_levels";
const JUTIL::ConstantString KILL_EATER_LEVELS_KEY = "levels";
const JUTIL::ConstantString KILL_EATER_LEVEL_KEY = "level";
const JUTIL::ConstantString KILL_EATER_NAME_KEY = "name";
const JUTIL::ConstantString KILL_EATER_SCORE_KEY = "required_score";

// Attribute JSON members.
const JUTIL::ConstantString ATTRIBUTE_NAME_KEY = "name";
const JUTIL::ConstantString ATTRIBUTE_INDEX_KEY = "defindex";
const JUTIL::ConstantString ATTRIBUTE_VALUE_KEY = "value";
const JUTIL::ConstantString ATTRIBUTE_MIN_VALUE_KEY = "min_value";
const JUTIL::ConstantString ATTRIBUTE_MAX_VALUE_KEY = "max_value";
const JUTIL::ConstantString ATTRIBUTE_EFFECT_TYPE_KEY = "effect_type";
const JUTIL::ConstantString ATTRIBUTE_IS_HIDDEN_KEY = "hidden";
const JUTIL::ConstantString ATTRIBUTE_IS_INTEGER_KEY = "stored_as_integer";
const JUTIL::ConstantString ATTRIBUTE_DESCRIPTION_KEY = "description_string";
const JUTIL::ConstantString ATTRIBUTE_DESCRIPTION_FORMAT_KEY = "description_format";

// Item JSON members.
const JUTIL::ConstantString ITEM_INDEX_KEY = "defindex";
const JUTIL::ConstantString ITEM_NAME_KEY = "item_name";
const JUTIL::ConstantString ITEM_IMAGE_KEY = "image_inventory";
const JUTIL::ConstantString ITEM_IMAGE_OVERLAY_KEY = "image_inventory_overlay";
const JUTIL::ConstantString ITEM_IMAGE_URL_KEY = "image_url";
const JUTIL::ConstantString ITEM_SLOT_KEY = "item_slot";
const JUTIL::ConstantString ITEM_CLASSES_KEY = "used_by_classes";
const JUTIL::ConstantString ITEM_ATTRIBUTES_KEY = "attributes";

// Tool JSON members
const JUTIL::ConstantString ITEM_TOOL_KEY = "tool";
const JUTIL::ConstantString ITEM_TOOL_TYPE_KEY = "type";
const JUTIL::ConstantString ITEM_TOOL_USAGE_CAPABILITIES_KEY = "usage_capabilities";
const JUTIL::ConstantString ITEM_TOOL_PAINTABLE_VALUE = "paintable";
const JUTIL::ConstantString ITEM_TOOL_PAINTABLE_TEAM_COLORS_VALUE = "paintable_team_colors";	
const JUTIL::ConstantString TOOL_TYPE_GIFT_VALUE = "gift";
const JUTIL::ConstantString TOOL_TYPE_DUEL_MINIGAME_VALUE = "duel_minigame";
const JUTIL::ConstantString TOOL_TYPE_NOISE_MAKER_VALUE = "noise_maker";
const JUTIL::ConstantString TOOL_TYPE_NAME_TAG_VALUE = "name";
const JUTIL::ConstantString TOOL_TYPE_KEY_VALUE = "decoder_ring";
const JUTIL::ConstantString TOOL_TYPE_CRATE_VALUE = "";
const JUTIL::ConstantString TOOL_TYPE_PAINT_CAN_VALUE = "paint_can";
const JUTIL::ConstantString TOOL_TYPE_DECAL_VALUE = "customize_texture_item";
const JUTIL::ConstantString TOOL_TYPE_GIFT_WRAP_VALUE = "gift_wrap";
const JUTIL::ConstantString TOOL_TYPE_WRAPPED_GIFT_VALUE = "wrapped_gift";
const JUTIL::ConstantString TOOL_TYPE_DESCRIPTION_TAG_VALUE = "desc";
const JUTIL::ConstantString TOOL_TYPE_BACKPACK_EXPANDER_VALUE = "backpack_expander";
const JUTIL::ConstantString TOOL_TYPE_WEDDING_RING_VALUE = "wedding_ring";
const JUTIL::ConstantString TOOL_TYPE_ACCOUNT_UPGRADE_VALUE = "account_upgrade_to_premium";
const JUTIL::ConstantString TOOL_TYPE_CLAIMCODE_VALUE = "claimcode";
const JUTIL::ConstantString TOOL_TYPE_STRANGE_PART_VALUE = "strange_part";
const JUTIL::ConstantString TOOL_TYPE_STRANGE_PART_RESTRICTION_VALUE = "strange_part_restriction";
const JUTIL::ConstantString TOOL_TYPE_SPELL_VALUE = "apply_custom_attrib";

/*
 * Get the member from a node if it exists.
 */
bool get_member( Json::Value* root, const JUTIL::String* member, Json::Value** output )
{
    // Verify that member exists.
    if (!root->isMember( member->get_string() )) {
        return false;
    }

    // Check that member is not null.
    Json::Value* element = &(*root)[ member->get_string() ];
    if (element->isNull()) {
        return false;
    }

    *output = element;
    return true;
}

/*
 * Definition loader constructor by graphics object.
 */
DefinitionLoader::DefinitionLoader( JUI::Graphics2D* graphics, ItemSchema* schema, NotificationQueue* notifications ) : 
	graphics_( graphics ),
	schema_( schema ),
	notifications_( notifications ),
	thread_( nullptr ),
	mutex_(),
	items_root_(),
	overview_root_(),
	classes_(),
	tools_(),
	name_map_(),
	progress_( 0.f ),
	state_( LOADING_STATE_NONE ),
	progress_message_(),
	has_state_changed_( false )
{
}

/*
 * Definition loader destructor.
 */
DefinitionLoader::~DefinitionLoader( void )
{
	if (thread_ != nullptr) {
		end();
        JUTIL::BaseAllocator::destroy( thread_ );
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
	thread_ = new (thread_) std::thread( std::bind( &DefinitionLoader::load, this ) );
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

	// Create class map.
	bool success;
    success = classes_.insert( &CLASS_NONE_NAME, INVENTORY_CLASS_NONE ) &&
        classes_.insert( &CLASS_SCOUT_NAME, INVENTORY_CLASS_SCOUT ) &&
        classes_.insert( &CLASS_SOLDIER_NAME, INVENTORY_CLASS_SOLDIER ) &&
        classes_.insert( &CLASS_PYRO_NAME, INVENTORY_CLASS_PYRO ) &&
        classes_.insert( &CLASS_DEMOMAN_NAME, INVENTORY_CLASS_DEMOMAN ) &&
        classes_.insert( &CLASS_HEAVY_NAME, INVENTORY_CLASS_HEAVY ) &&
        classes_.insert( &CLASS_ENGINEER_NAME, INVENTORY_CLASS_ENGINEER ) &&
        classes_.insert( &CLASS_MEDIC_NAME, INVENTORY_CLASS_MEDIC ) &&
        classes_.insert( &CLASS_SNIPER_NAME, INVENTORY_CLASS_SNIPER ) &&
        classes_.insert( &CLASS_SPY_NAME, INVENTORY_CLASS_SPY );
    if (!success) {
        stack->log( "Failed to create class map.");
        return false;
    }

	// Create tool map.
	success = tools_.insert( &TOOL_TYPE_GIFT_VALUE, TOOL_GIFT ) &&
		tools_.insert( &TOOL_TYPE_DUEL_MINIGAME_VALUE, TOOL_DUEL_MINIGAME ) &&
		tools_.insert( &TOOL_TYPE_NOISE_MAKER_VALUE, TOOL_NOISE_MAKER ) &&
		tools_.insert( &TOOL_TYPE_NAME_TAG_VALUE, TOOL_NAME_TAG ) &&
		tools_.insert( &TOOL_TYPE_KEY_VALUE, TOOL_KEY ) &&
		tools_.insert( &TOOL_TYPE_CRATE_VALUE, TOOL_CRATE ) &&
		tools_.insert( &TOOL_TYPE_PAINT_CAN_VALUE, TOOL_PAINT_CAN ) &&
		tools_.insert( &TOOL_TYPE_DECAL_VALUE, TOOL_DECAL ) &&
		tools_.insert( &TOOL_TYPE_GIFT_WRAP_VALUE, TOOL_GIFT_WRAP ) &&
		tools_.insert( &TOOL_TYPE_WRAPPED_GIFT_VALUE, TOOL_WRAPPED_GIFT ) &&
		tools_.insert( &TOOL_TYPE_DESCRIPTION_TAG_VALUE, TOOL_DESCRIPTION_TAG ) &&
		tools_.insert( &TOOL_TYPE_BACKPACK_EXPANDER_VALUE, TOOL_BACKPACK_EXPANDER ) &&
		tools_.insert( &TOOL_TYPE_WEDDING_RING_VALUE, TOOL_WEDDING_RING ) &&
		tools_.insert( &TOOL_TYPE_ACCOUNT_UPGRADE_VALUE, TOOL_ACCOUNT_UPGRADE ) &&
		tools_.insert( &TOOL_TYPE_CLAIMCODE_VALUE, TOOL_CLAIMCODE ) &&
		tools_.insert( &TOOL_TYPE_STRANGE_PART_VALUE, TOOL_STRANGE_PART ) &&
		tools_.insert( &TOOL_TYPE_STRANGE_PART_RESTRICTION_VALUE, TOOL_STRANGE_PART ) &&
		tools_.insert( &TOOL_TYPE_SPELL_VALUE, TOOL_SPELL );

	set_state( LOADING_STATE_DOWNLOAD_DEFINITIONS );

	// Get downloader instance.
    JUI::FileDownloader* downloader = JUI::FileDownloader::get_instance();
    if (downloader == nullptr) {
        stack->log( "Failed to create downloader.");
        set_state( LOADING_STATE_ERROR );
        return false;
    }

	// Get item definition file.
	constexpr long definition_load_timeout = 30;
	downloader->set_timeout( definition_load_timeout );
    JUTIL::DynamicString items_text;
	bool succeeded = downloader->read_cached( &SCHEMA_ITEMS_FILE_LOCATION, &SCHEMA_ITEMS_URL, &items_text ) || downloader->read( &SCHEMA_ITEMS_FILE_LOCATION, &items_text );
	downloader->clear_timeout();
    if(!succeeded)
	{
		stack->log( "Failed to read schema items from Steam web API.");
		set_state( LOADING_STATE_ERROR );
		return false;
    }

	// Get overview definition file.
	downloader->set_timeout( definition_load_timeout );
    JUTIL::DynamicString overview_text;
	succeeded = downloader->read_cached( &SCHEMA_OVERVIEW_FILE_LOCATION, &SCHEMA_OVERVIEW_URL, &overview_text ) || downloader->read( &SCHEMA_OVERVIEW_FILE_LOCATION, &overview_text );
	downloader->clear_timeout();
    if(!succeeded)
	{
		stack->log( "Failed to read schema overview from Steam web API.");
		set_state( LOADING_STATE_ERROR );
		return false;
    }

	// Parse item definition file.
	Json::CharReaderBuilder builder;
	Json::CharReader* reader = builder.newCharReader();
	const char* items_definition_start = items_text.get_string();
	const char* items_definition_end = items_definition_start + items_text.get_length();
	if (!reader->parse( items_definition_start, items_definition_end, &items_root_, nullptr )) {
        stack->log( "Failed to parse item definition JSON.");
        set_state( LOADING_STATE_ERROR );
        return false;
	}

	// Parse overview definition file.
	std::string errors;
	reader = builder.newCharReader();
	const char* overview_definition_start = overview_text.get_string();
	const char* overview_definition_end = overview_definition_start + overview_text.get_length();
	if (!reader->parse( overview_definition_start, overview_definition_end, &overview_root_, &errors )) {
        stack->log( "Failed to parse overview definition JSON.");
        set_state( LOADING_STATE_ERROR );
        return false;
	}

    // Try load definitions.
    if (!load_definitions( &items_root_, &overview_root_ )) {
        set_state( LOADING_STATE_ERROR );
        return false;
    }

	clean_up();
	set_state( LOADING_STATE_FINISHED );
    return true;
}

/*
 * Load definitions from JSON root object.
 */
bool DefinitionLoader::load_definitions( Json::Value* items_root, Json::Value* overview_root )
{
    // Stack for reporting.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Get results from both.
    Json::Value* items_result;
    if (!get_member( &items_root_, &RESULT_KEY, &items_result )) {
        stack->log( "Failed to get result from item definitions." );
        return false;
    }
	Json::Value* overview_result;
	if (!get_member( &overview_root_, &RESULT_KEY, &overview_result )) {
		stack->log( "Failed to get result from overview definition." );
		return false;
	}
		
	// Start loading attributes.
	if (!load_attributes( overview_result )) {
		return false;
	}

	// Load quality names.
	if (!load_qualities( overview_result )) {
		return false;
	}

	// Load origin names.
	if (!load_origins( overview_result )) {
		return false;
	}

	// Load kill eater ranks.
	if (!load_kill_eater_ranks( overview_result )) {
		return false;
	}

	// Load kill eater types.
	if (!load_kill_eater_types( overview_result )) {
		return false;
	}

	// Load item definitions.
	if (!load_items( items_result )) {
		return false;
	}

    // Get fallback texture.
    JUI::FileDownloader* downloader = JUI::FileDownloader::get_instance();
    if (!downloader->check_and_get( &FALLBACK_ITEM_TEXTURE, &FALLBACK_ITEM_TEXTURE_URL )) {
        stack->log( "Failed to download fallback item texture!" );
        return false;
    }

	// Create fallback definition.
    JUI::FileTexture* unknown_item;
    JUI::Graphics2D::ReturnStatus status;
    status = graphics_->get_texture( &FALLBACK_ITEM_TEXTURE, &unknown_item );
    if (status != JUI::Graphics2D::Success) {
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
    ItemDefinition* fallback;
    if (!JUTIL::BaseAllocator::allocate( &fallback )) {
        JUTIL::BaseAllocator::destroy( unknown_name );
        stack->log( "Failed to create fallback item definition.");
        return false;
    }

	// Make a copy of the fallback texture path to give this definition.
	JUTIL::DynamicString fallback_image;
	if (!fallback_image.copy( &FALLBACK_ITEM_TEXTURE )) {
		JUTIL::BaseAllocator::allocate( &fallback );
		stack->log( "Failed to make a copy of fallback image path.");
		return false;
	}
	fallback = new (fallback) ItemDefinition(
		unknown_name,
		&fallback_image,
		FALLBACK_ITEM_CLASS_FLAGS );
    schema_->set_fallback_definition( fallback );

    // Make sure item loading succeeded.
	schema_->set_loaded( true );
    return true;
}

/*
 * Load the attribute definitions into the schema.
 */
bool DefinitionLoader::load_attributes( Json::Value* result )
{
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Get the attributes member.
	Json::Value* attributes;
	if (!get_member( result, &ATTRIBUTES_KEY, &attributes )) {
		stack->log( "Failed to get attributes from definition." );
		return false;
	}

	// Begin loading attributes.
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
        Json::Value* attribute_is_hidden;
        Json::Value* attribute_is_integer;
        bool success = get_member( &attribute, &ATTRIBUTE_NAME_KEY, &attribute_name ) &&
            get_member( &attribute, &ATTRIBUTE_INDEX_KEY, &attribute_index ) &&
            get_member( &attribute, &ATTRIBUTE_IS_HIDDEN_KEY, &attribute_is_hidden ) &&
            get_member( &attribute, &ATTRIBUTE_IS_INTEGER_KEY, &attribute_is_integer );
        if (!success) {
            stack->log( "Unexpected attribute format found." );
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

        // Get rest of parameters and create information object.
        unsigned int index = attribute_index->asUInt();

		// Min/max values removed since was inconsistent in schema.
        bool is_hidden = attribute_is_hidden->asBool();
        bool is_integer = attribute_is_integer->asBool();

        // Allocate information object.
        AttributeDefinition* attrib_info;
        if (!JUTIL::BaseAllocator::allocate( &attrib_info )) {
            JUTIL::BaseAllocator::destroy( name_string );
            return false;
        }
		attrib_info = new (attrib_info) AttributeDefinition(
			name_string,
            index,
			is_hidden,
			is_integer );

		// Check for optional members.
        Json::Value* attribute_description;
        Json::Value* attribute_format;
        bool has_description = get_member( &attribute, &ATTRIBUTE_DESCRIPTION_KEY, &attribute_description ) &&
            get_member( &attribute, &ATTRIBUTE_DESCRIPTION_FORMAT_KEY, &attribute_format );
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
        if (!schema_->add_attribute_definition( index, attrib_info )) {
            JUTIL::BaseAllocator::destroy( attrib_info );
            stack->log( "Failed to add attribute to schema.");
            return false;
        }
        else if (!name_map_.insert( name_string, static_cast<uint16>(index) )) {
            // Will be destroyed by item attributes, no need to delete.
            stack->log( "Failed to add attribute index to name map.");
            return false;
        }

		loaded_attribs++;
		set_progress( loaded_attribs, num_attribs );
	}

	return true;
}

/*
 * Load the quality names/indices into the schema.
 */
bool DefinitionLoader::load_qualities( Json::Value* result )
{
	// Error stack for reporting.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Get qualities from result.
	Json::Value* qualities;
	Json::Value* quality_names;
	if (!get_member( result, &QUALITIES_KEY, &qualities )) {
		stack->log( "Failed to find qualities in definitions." );
		return false;
	}
	if (!get_member( result, &QUALITY_NAMES_KEY, &quality_names )) {
		stack->log( "Failed to find quality names in definitions." );
		return false;
	}

	// Go through each quality and map the index to its full name.
	for (Json::ValueIterator i = qualities->begin(); i != qualities->end(); ++i) {
		// Get the token for the quality index entry.
		const JSONCPP_STRING json_name = i.name();
		const JUTIL::ConstantString quality_token = json_name.c_str();
		unsigned int index = (*i).asUInt();

		// Find the matching quality string.
		Json::Value* quality_name;
		if (!get_member( quality_names, &quality_token, &quality_name )) {
			stack->log( "Failed to find matching quality name for index." );
			return false;
		}

		// Insert mapping for index to name.
		JUTIL::DynamicString* name;
		if (!JUTIL::BaseAllocator::allocate( &name )) {
			stack->log( "Failed to allocate string to store quality name." );
			return false;
		}
		new (name) JUTIL::DynamicString();
		if (!name->write( "%s", quality_name->asCString() )) {
			JUTIL::BaseAllocator::destroy( name );
			stack->log( "Failed to write quality name to string." );
			return false;
		}
		if (!schema_->add_quality_name( index, name )) {
			JUTIL::BaseAllocator::destroy( name );
			stack->log( "Failed to add quality to schema.");
			return false;
		}
	}
	return true;
}

/*
 * Load the origin names into the schema.
 */
bool DefinitionLoader::load_origins( Json::Value* result )
{
	// Stack for reporting errors.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Get the origins member.
	Json::Value* origins;
	if (!get_member( result, &ORIGIN_NAMES_KEY, &origins )) {
		stack->log( "Failed to find origins in definitions." );
		return false;
	}

	// Go through each origin and add to schema.
	for (Json::ValueIterator o = origins->begin(); o != origins->end(); ++o) {
		Json::Value& origin = *o;
		Json::Value* origin_index;
		Json::Value* origin_name;

		// Get index and name.
		if (!get_member( &origin, &ORIGIN_INDEX_KEY, &origin_index )) {
			stack->log( "Failed to find origin index in definitions." );
			return false;
		}
		if (!get_member( &origin, &ORIGIN_NAME_KEY, &origin_name )) {
			stack->log( "Failed to find origin index in definitions." );
			return false;
		}
		unsigned int index = (*origin_index).asUInt();

		// Allocate new name string.
        JUTIL::DynamicString* origin_name_string;
        if (!JUTIL::BaseAllocator::allocate( &origin_name_string )) {
			stack->log( "Failed to allocate origin name string." );
            return false;
        }
        origin_name_string = new (origin_name_string) JUTIL::DynamicString();
		if (!origin_name_string->write( "%s", origin_name->asCString() )) {
			JUTIL::BaseAllocator::destroy( origin_name_string );
			stack->log( "Failed to write copy of origin name." );
			return false;
		}
		if (!schema_->add_origin_name( index, origin_name_string )) {
            JUTIL::BaseAllocator::destroy( origin_name_string );
            stack->log( "Failed to add origin to schema.");
            return false;
        }
	}

	return true;
}

/*
 * Load the kill eater ranks into the schema.
 */
bool DefinitionLoader::load_kill_eater_ranks( Json::Value* result )
{
	// Stack for reporting errors.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Get the item levels element.
	Json::Value* ranks;
	if(!get_member( result, &KILL_EATER_RANKS_KEY, &ranks )) {
		stack->log( "Failed to find strange index in definitions." );
		return false;
	}

	// Go through each kill eater class.
	for (Json::ValueIterator k = ranks->begin(); k != ranks->end(); ++k) {
		Json::Value& rank = *k;
		Json::Value* class_name;
		Json::Value* levels;

		// Get the class name and get its levels.
		if (!get_member( &rank, &KILL_EATER_CLASS_KEY, &class_name )) {
			stack->log( "Failed to find strange index in definitions." );
			return false;
		}
		if (!get_member( &rank, &KILL_EATER_LEVELS_KEY, &levels )) {
			stack->log( "Failed to find strange index in definitions." );
			return false;
		}

		// Get the rank name.
		JUTIL::ConstantString rank_name = class_name->asCString();

		// Allocate information object.
		KillEaterRanks* rank_info;
        if (!JUTIL::BaseAllocator::allocate( &rank_info )) {
			stack->log( "Failed to allocate kill eater rank object." );
            return false;
        }
		new (rank_info) KillEaterRanks();

		// Go through each level and add to this class.
		for (Json::ValueIterator l = levels->begin(); l != levels->end(); ++l) {
			Json::Value& kill_eater_level = *l;
			Json::Value* level_index;
			Json::Value* level_score;
			Json::Value* level_prefix;

			// Get the level, minimum score, and name.
			bool success = get_member( &kill_eater_level, &KILL_EATER_LEVEL_KEY, &level_index ) &&
				get_member( &kill_eater_level, &KILL_EATER_SCORE_KEY, &level_score ) &&
				get_member( &kill_eater_level, &KILL_EATER_NAME_KEY, &level_prefix );
			if (!success) {
				stack->log( "Failed to get kill eater level members." );
				JUTIL::BaseAllocator::destroy( rank_info );
				return false;
			}

			// Allocate new prefix string.
			JUTIL::DynamicString* prefix;
			if (!JUTIL::BaseAllocator::allocate( &prefix )) {
				JUTIL::BaseAllocator::destroy( rank_info );
				stack->log( "Failed to allocate level prefix string." );
				return false;
			}
			new (prefix) JUTIL::DynamicString();
			if (!prefix->write( "%s", level_prefix->asCString() )) {
				JUTIL::BaseAllocator::destroy( prefix );
				JUTIL::BaseAllocator::destroy( rank_info );
				stack->log( "Failed to write level prefix." );
				return false;
			}

			// Create an object for this level.
			unsigned int score = level_score->asUInt();
			KillEaterLevel* level_info;
			if (!JUTIL::BaseAllocator::allocate( &level_info )) {
				JUTIL::BaseAllocator::destroy( prefix  );
				JUTIL::BaseAllocator::destroy( rank_info );
				stack->log( "Failed to allocate kill eater level object." );
				return false;
			}
			new (level_info) KillEaterLevel( score, prefix );
			if (!rank_info->add_level( level_info )) {
				JUTIL::BaseAllocator::destroy( level_info );
				JUTIL::BaseAllocator::destroy( rank_info );
				stack->log( "Failed to add kill eater level object to rank." );
				return false;
			}
		}

		// Add this rank to the schema.
		if (!schema_->add_kill_eater_ranks( &rank_name, rank_info )) {
            JUTIL::BaseAllocator::destroy( rank_info );
            stack->log( "Failed to add strange rank class to schema.");
            return false;
        }
	}

	return true;
}

/*
 * Load the kill eater types into the schema.
 */
bool DefinitionLoader::load_kill_eater_types( Json::Value* result )
{
	// Stack for reporting errors.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Get the types from the schema.
	Json::Value* types;
	if(!get_member( result, &KILL_EATER_TYPE_KEY, &types )) {
		stack->log( "Failed to find strange index in definitions." );
		return false;
	}
	for (Json::ValueIterator t = types->begin(); t != types->end(); ++t) {
		Json::Value& type = *t;
		Json::Value* type_index;
		Json::Value* type_name;
		Json::Value* type_level_class;

		// Get the type name, description, and level type.
		if (!get_member( &type, &KILL_EATER_TYPE_INDEX_KEY, &type_index )) {
			stack->log( "Failed to find strange index in definitions." );
			return false;
		}
		if (!get_member( &type, &KILL_EATER_TYPE_NAME_KEY, &type_name )) {
			stack->log( "Failed to find strange index in definitions." );
			return false;
		}
		if (!get_member( &type, &KILL_EATER_TYPE_LEVEL_DATA_KEY, &type_level_class )) {
			stack->log( "Failed to find strange index in definitions." );
			return false;
		}

		// Write the level data to a string.
		const JUTIL::ConstantString level_class = type_level_class->asCString();
		
		// Find the rank by name.
		const KillEaterRanks* ranks = schema_->get_kill_eater_ranks( &level_class );
		if (ranks == nullptr) {
			stack->log( "Failed to match strange text in definitions." );
			return false;
		}

		// Allocate new description string.
		JUTIL::DynamicString* name;
		if (!JUTIL::BaseAllocator::allocate( &name )) {
			stack->log( "Failed to allocate kill eater type name string." );
			return false;
		}
		new (name) JUTIL::DynamicString();
		if (!name->write( "%s", type_name->asCString() )) {
			JUTIL::BaseAllocator::destroy( name );
			stack->log( "Failed to write kill eater type name to string." );
			return false;
		}

		// Allocate information object.
		KillEaterType* kill_eater_type;
        if (!JUTIL::BaseAllocator::allocate( &kill_eater_type )) {
			JUTIL::BaseAllocator::destroy( name );
			stack->log( "Failed to create kill eater type object." );
            return false;
        }
		new (kill_eater_type) KillEaterType( name, ranks );

		// Add to the schema.
		unsigned int index = type_index->asUInt();
		if (!schema_->add_kill_eater_type( index, kill_eater_type )) {
            JUTIL::BaseAllocator::destroy( kill_eater_type );
            stack->log( "Failed to add kill eater type to schema.");
            return false;
        }
	}

	return true;
}

/*
 * Load item definitions into the schema.
 */
bool DefinitionLoader::load_items( Json::Value* result )
{
	// Stack for reporting errors.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Get item member.
    Json::Value* items;
    if (!get_member( result, &ITEMS_KEY, &items )) {
        stack->log( "Failed to find item object in definitions.");
        return false;
    }

	// Start loading items.
	bool success = true;
	size_t loaded_items = 0;
	size_t num_items = items->size();
	set_state( LOADING_STATE_LOADING_ITEMS );
	for (Json::ValueIterator i = items->begin(); i != items->end(); ++i) {
		// Return if we're exiting.
		if (should_stop_) {
			return false;
		}

        // Allocate strings for parsing.
		JUTIL::DynamicString* name;
        if (!JUTIL::BaseAllocator::allocate( &name )) {
            stack->log( "Failed to allocate string for item name." );
			return false;
        }
        new (name) JUTIL::DynamicString();

        // Image and URL strings.
        JUTIL::DynamicString image;
        JUTIL::DynamicString image_url;

        // Load item.
		Json::Value* item = &(*i);
        if (!load_item( item, name, &image, &image_url )) {
            JUTIL::BaseAllocator::destroy( name );
			return false;
        }

		// Update progress.
		loaded_items++;
		set_progress( loaded_items, num_items );
	}

	return true;
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
    if (!get_member( item, &ITEM_INDEX_KEY, &item_index ) || !get_member( item, &ITEM_NAME_KEY, &item_name )) {
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
    bool use_fallback = true;
    Json::Value* item_image;
    Json::Value* item_image_url;
    if (get_member( item, &ITEM_IMAGE_KEY, &item_image ) && get_member( item, &ITEM_IMAGE_URL_KEY, &item_image_url)) {
        // Write image file.
        const JUTIL::ConstantString IMAGE = item_image->asCString();
        const JUTIL::ConstantString IMAGE_URL = item_image_url->asCString();
        if ((IMAGE.get_length() != 0) && (IMAGE_URL.get_length() != 0)) {
            if (!image->write( "img/%s.png", IMAGE.get_string() )) {
		        stack->log( "Failed to write image path for item definition." );
		        return false;
	        }
	        else if (!image_url->copy( &IMAGE_URL )) {
                stack->log( "Failed to create image or URL string for item definition." );
                return false;
            }

            // Successfully wrote.
            use_fallback = false;
        }
    }
    if (use_fallback) {
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

	// Get classes, if they exist.
    Json::Value* item_classes;
	unsigned int classes = INVENTORY_CLASS_ALL;
    if (get_member( item, &ITEM_CLASSES_KEY, &item_classes )) {
        size_t i;
        size_t length = item_classes->size();
        for (i = 0; i < length; ++i) {
            const JUTIL::ConstantString class_name = (*item_classes)[i].asCString();

            // Find enum for name.
            EInventoryClass single_class;
            if (!classes_.get( &class_name, &single_class )) {
                stack->log( "Unknown class type found in definitions.");
                return false;
            }

            classes |= static_cast<unsigned int>(single_class);
        }
    }

    // Load texture; fall back to default if failed.
    JUI::FileDownloader* downloader = JUI::FileDownloader::get_instance();

	// Best effort to download the file; use fallback if failed.
    if (!downloader->check_and_get( image, image_url )) {
		if (!image->copy( &FALLBACK_ITEM_TEXTURE )) {
			stack->log( "Failed to copy fallback item texture name." );
            return false;
        }
	}
    
	// Generate information object.
    ItemDefinition* information;
    if (!JUTIL::BaseAllocator::allocate( &information )) {
        stack->log( "Failed to create item information object.");
        return false;
    }
    information = new (information) ItemDefinition( name, image, classes );

	// Load tool info and alternate textures
	Json::Value* tool;
	Json::Value* tool_type;
	if (get_member( item, &ITEM_TOOL_KEY, &tool )) {
		if (get_member( tool, &ITEM_TOOL_TYPE_KEY, &tool_type )) {
			JUTIL::ConstantString tool_type_name = tool_type->asCString();
			EItemToolType tool_type_value;
			if (tools_.get( &tool_type_name, &tool_type_value )) {
				information->set_tool_type( tool_type_value );
			}
		}
    }

	// Now add attributes, if they exist.
    Json::Value* item_attributes;
    if (get_member( item, &ITEM_ATTRIBUTES_KEY, &item_attributes )) {
        // Iterate through all attributes.
		for (Json::ValueIterator j = item_attributes->begin(); j != item_attributes->end(); ++j) {
			Json::Value* attribute = &(*j);

			// Get class and value.
            if (!load_item_attribute( attribute, information )) {
                return false;
            }
		}
    }

	// Parse item type and insert.
    uint16 index = static_cast<uint16>(item_index->asUInt());
    if (!schema_->add_item_definition( index, information )) {
        JUTIL::BaseAllocator::destroy( information );
        stack->log( "Failed to add item information to schema.");
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
bool DefinitionLoader::load_item_attribute( Json::Value* attribute, ItemDefinition* information )
{
	// Get error stack for reporting failure.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Get attribute name.
    Json::Value* attribute_name;
    Json::Value* attribute_value;
    if (!get_member( attribute, &ATTRIBUTE_NAME_KEY, &attribute_name )) {
        stack->log( "Failed to get name from attribute definition.");
        return false;
    }
    else if (!get_member( attribute, &ATTRIBUTE_VALUE_KEY, &attribute_value )) {
        stack->log( "Failed to get value from attribute definition.");
        return false;
    }
    const JUTIL::ConstantString name( attribute_name->asCString() );
	float value = attribute_value->asFloat();

	// Get information.
	uint16 attribute_index;
	if (!name_map_.get( &name, &attribute_index )) {
        stack->log( "Failed to find attribute index by name.");
        return false;
	}

	// Create new attribute.
    Attribute* new_attribute;
    if (!JUTIL::BaseAllocator::allocate( &new_attribute )) {
        stack->log( "Failed to create attribute.");
        return false;
    }
    new_attribute = new (new_attribute) Attribute( attribute_index );
    if (!information->add_attribute( new_attribute )) {
		JUTIL::BaseAllocator::destroy( &new_attribute );
        stack->log( "Failed to add attribute to item definition.");
        return false;
    }
	if (!new_attribute->set_value( reinterpret_cast<const char*>(&value), sizeof(float) )) {
		stack->log( "Failed to copy value of attribute." );
		return false;
	}

	// Resolve attribute from schema.
	if (!schema_->resolve( new_attribute )) {
		JUTIL::BaseAllocator::destroy( &new_attribute );
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
	items_root_.clear();
	overview_root_.clear();
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
		if (!progress_message_.write( "Loading attributes from schema...\n%.2f%%", progress )) {
			return false;
		}
		break;

	case LOADING_STATE_LOADING_ITEMS:
		if (!progress_message_.write( "Loading items from schema...\n%.2f%%", progress )) {
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