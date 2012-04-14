#include "definition_loader.h"

#include <jui/file_downloader.h>

Json::Value& get_member( Json::Value& root, const std::string& member )
{
	if (!root.isMember( member )) {
		throw std::runtime_error( "Unexpected definition format, key '" + member + "' not found." );
	}

	return root[member];
}

DefinitionLoader::DefinitionLoader( Graphics2D *graphics )
{
	graphics_ = graphics;

	set_progress( 0.0f );
	set_state( LOADING_STATE_NONE );
}

DefinitionLoader::~DefinitionLoader()
{
	if (thread_ != nullptr) {
		end();
	}
}

void DefinitionLoader::begin()
{
	assert( thread_ == nullptr );
	stop_ = false;
	set_state( LOADING_STATE_START );
	thread_ = boost::shared_ptr<boost::thread>( new boost::thread( boost::bind( &DefinitionLoader::load, this ) ) );
}

void DefinitionLoader::end()
{
	assert( thread_ != nullptr );
	stop_ = true;
	thread_->join();
}

void DefinitionLoader::load()
{
	// Set this thread's render context.
	graphics_->set_render_context( graphics_->get_loading_context() );

	// Create slot name map.
	slots_[""]			= SLOT_NONE;
	slots_["invalid"]	= SLOT_INVALID;
	slots_["primary"]	= SLOT_PRIMARY;
	slots_["secondary"]	= SLOT_SECONDARY;
	slots_["melee"]		= SLOT_MELEE;
	slots_["pda"]		= SLOT_PDA;
	slots_["pda2"]		= SLOT_PDA2;
	slots_["building"]	= SLOT_BUILDING;
	slots_["head"]		= SLOT_HEAD;
	slots_["misc"]		= SLOT_MISC;
	slots_["action"]	= SLOT_ACTION;
	slots_["grenade"]	= SLOT_GRENADE;

	// Create class map.
	classes_[""]		= CLASS_NONE;
	classes_["Scout"]	= CLASS_SCOUT;
	classes_["Soldier"]	= CLASS_SOLDIER;
	classes_["Pyro"]	= CLASS_PYRO;
	classes_["Demoman"]	= CLASS_DEMOMAN;
	classes_["Heavy"]	= CLASS_HEAVY;
	classes_["Engineer"] = CLASS_ENGINEER;
	classes_["Medic"]	= CLASS_MEDIC;
	classes_["Sniper"]	= CLASS_SNIPER;
	classes_["Spy"]		= CLASS_SPY;

	set_state( LOADING_STATE_DOWNLOAD_DEFINITIONS );

	// Get downloader instance.
	FileDownloader* downloader = nullptr;

	try {
		// Get definition file.
		std::string definition;
		try {
			downloader = FileDownloader::get_instance();
			definition = downloader->read( "http://api.steampowered.com/IEconItems_440/GetSchema/v0001/?key=0270F315C25E569307FEBDB67A497A2E&format=json&language=en" );
		}
		catch (std::runtime_error&) {
			throw std::runtime_error( "Failed to read item definition file from Steam Web API." );
		}

		// Parse definition file.
		Json::Reader reader;
		if (!reader.parse( definition, root_, false )) {
			throw std::runtime_error( "Failed to parse item definition file." );
		}

		Json::Value& result = get_member( root_, "result" );
		Json::Value& attributes = get_member( result, "attributes" );

		// Create table of class/attribute pairs.
		std::hash_map<std::string, AttributeInformation*, StringHasher> name_map;
		
		// Start loading attributes.
		size_t loaded_attribs = 0;
		size_t num_attribs = attributes.size();
		set_state( LOADING_STATE_LOADING_ATTRIBUTES );
		for (Json::ValueIterator i = attributes.begin(); i != attributes.end(); ++i) {
			// Quit if we need to stop.
			if (stop_) {
				return;
			}

			Json::Value& attribute = *i;

			// Get necessary members.
			std::string name = get_member( attribute, "name" ).asString();
			uint32 index = get_member( attribute, "defindex" ).asUInt();
			std::string attribute_class = get_member( attribute, "attribute_class" ).asString();
			float min_value = static_cast<float>(get_member( attribute, "min_value" ).asDouble());
			float max_value = static_cast<float>(get_member( attribute, "max_value" ).asDouble());
			std::string effect_type_name = get_member( attribute, "effect_type" ).asString();
			bool hidden = get_member( attribute, "hidden" ).asBool();
			bool is_integer = get_member( attribute, "stored_as_integer" ).asBool();

			// Get effect type, third char is different for each time.
			EffectType effect_type;
			if (effect_type_name == "positive") {
				effect_type = EFFECT_POSITIVE;
			}
			else if (effect_type_name == "negative") {
				effect_type = EFFECT_NEGATIVE;
			}
			else if (effect_type_name == "neutral") {
				effect_type = EFFECT_NEUTRAL;
			}
			else {
				throw std::runtime_error( "Unexpected effect type '" + effect_type_name + "' received." );
			}

			// Create new attribute definition.
			AttributeInformation* attrib_info = new AttributeInformation(
				name,
				index,
				attribute_class,
				min_value, max_value,
				effect_type,
				hidden,
				is_integer );

			// Check for optional members.
			if (attribute.isMember( "description_string" ) && attribute.isMember( "description_format" )) {
				std::string desc_string = attribute[ "description_string" ].asString();
				std::string desc_format = attribute[ "description_format" ].asString();
				attrib_info->set_description( desc_string, desc_format );
			}

			// Add to both maps.
			name_map[name] = attrib_info;
			Item::attributes[index] = attrib_info;

			loaded_attribs++;
			set_progress( loaded_attribs, num_attribs );
		}

		// Get item member.
		Json::Value& items = get_member( result, "items" );
		if (items == Json::Value::null) {
			throw std::runtime_error( "Unexpected definition format, expected result.items." );
		}

		// Create fallback definition.
		Texture* unknown_item = graphics_->get_texture( "img/backpack/unknown_item.png" );
		Item::fallback = new ItemInformation(
			"Unknown Item",
			unknown_item,
			0,
			SLOT_NONE );

		// Start loading items.
		size_t loaded_items = 0;
		size_t num_items = items.size();
		set_state( LOADING_STATE_LOADING_ITEMS );
		for (Json::ValueIterator i = items.begin(); i != items.end(); ++i) {
			// Return if we're exiting.
			if (stop_) {
				return;
			}

			Json::Value item = *i;

			// Check that all necessary attributes exist.
			unsigned int defindex		= get_member( item, "defindex" ).asUInt();
			std::string item_name		= get_member( item, "item_name" ).asString();
			std::string image_inventory	= get_member( item, "image_inventory" ).asString();
			std::string image_url		= get_member( item, "image_url" ).asString();

			// Fallback image.
			if (image_inventory.empty()) {
				image_inventory = "backpack/unknown_item";
			}

			// Fallback URL.
			if (image_url.empty()) {
				image_url = "http://www.jengerer.com/item_manager/img/" + image_inventory + ".png";
			}

			EItemSlot item_slot = SLOT_NONE;
			if (item.isMember( "item_slot" )) {
				std::string slot_name = item["item_slot"].asString();
				auto j = slots_.find( slot_name );
				if (j == slots_.end()) {
					throw std::runtime_error( "Failed to parse item definitions. Unexpected item slot type '" + slot_name + "' found." );
				}
				else {
					item_slot = j->second;
				}
			}

			// Get classes, if they exist.
			unsigned int item_classes = CLASS_ALL;
			if (item.isMember( "used_by_classes" )) {
				item_classes = CLASS_NONE;
				Json::Value classes = item["used_by_classes"];
				for (size_t i = 0; i < classes.size(); i++) {
					std::string class_name = classes[i].asString();

					auto k = classes_.find( class_name );
					if (k == classes_.end()) {
						throw std::runtime_error( "Failed to parse item definitions. Unexpected class type '" + class_name + "' found." );
					}

					item_classes |= k->second;
				}
			}

			// Get file name.
			std::string file_name = "img/" + image_inventory + ".png";

			// Check that texture exists.
			Texture* texture = nullptr;
			FileDownloader* downloader = FileDownloader::get_instance();
			try {
				downloader->check_and_get( file_name, image_url );
				texture = graphics_->get_texture( file_name );
			}
			catch (const std::runtime_error&) {
				texture = unknown_item;
			}

			// Generate information object.
			ItemInformation *itemInformation = new ItemInformation(
				item_name,
				texture,
				item_classes,
				item_slot );

			// Now add attributes, if they exist.
			if (item.isMember( "attributes" )) {
				Json::Value& item_attributes = item[ "attributes" ];
				for (Json::ValueIterator j = item_attributes.begin(); j != item_attributes.end(); ++j) {
					Json::Value& item_attrib = *j;

					// Get class and value.
					std::string attrib_name = get_member( item_attrib, "name" ).asString();
					float attrib_value = static_cast<float>(get_member( item_attrib, "value" ).asDouble());

					// Get information.
					AttributeInformation* attrib_info = name_map[attrib_name];
					if (attrib_info == nullptr) {
						throw std::runtime_error( "Failed to get attribute information for attribute '" + attrib_name + "'." );
					}

					// Create new attribute.
					Attribute* attribute = new Attribute( attrib_info, attrib_value );
					itemInformation->add_attribute( attribute );
				}
			}

			// Parse item type and insert.
			Item::definitions[defindex] = itemInformation;

			// Update progress.
			loaded_items++;
			set_progress( loaded_items, num_items );
		}

		clean_up();
		set_state( LOADING_STATE_FINISHED );
	}
	catch (std::runtime_error& exception) {
		clean_up();
		set_error( exception.what() );
	}
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