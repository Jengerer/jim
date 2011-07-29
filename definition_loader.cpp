#include "definition_loader.h"
#include "curl.h"

DefinitionLoader::DefinitionLoader( Graphics2D *graphics )
{
	stop_ = false;
	loaded_ = 0;

	graphics_ = graphics;

	SetProgress( 0.0f );
	SetState( LOADING_STATE_START );
}

void DefinitionLoader::Begin()
{
	assert( thread_ == nullptr );
	SetState( LOADING_STATE_RUNNING );
	thread_ = boost::shared_ptr<boost::thread>( new boost::thread( boost::bind( &DefinitionLoader::Load, this ) ) );
}

void DefinitionLoader::End()
{
	assert( thread_ != nullptr );
	stop_ = true;
	thread_->join();
}

void DefinitionLoader::Load()
{
	graphics_->set_render_context( graphics_->get_loading_context() );

	// Create slot name map.
	slotTypes_[""]			= SLOT_NONE;
	slotTypes_["invalid"]	= SLOT_INVALID;
	slotTypes_["primary"]	= SLOT_PRIMARY;
	slotTypes_["secondary"]	= SLOT_SECONDARY;
	slotTypes_["melee"]		= SLOT_MELEE;
	slotTypes_["pda"]		= SLOT_PDA;
	slotTypes_["pda2"]		= SLOT_PDA2;
	slotTypes_["building"]	= SLOT_BUILDING;
	slotTypes_["head"]		= SLOT_HEAD;
	slotTypes_["misc"]		= SLOT_MISC;
	slotTypes_["action"]	= SLOT_ACTION;
	slotTypes_["grenade"]	= SLOT_GRENADE;

	// Create class map.
	classTypes_[""]			= CLASS_NONE;
	classTypes_["Scout"]	= CLASS_SCOUT;
	classTypes_["Soldier"]	= CLASS_SOLDIER;
	classTypes_["Pyro"]		= CLASS_PYRO;
	classTypes_["Demoman"]	= CLASS_DEMOMAN;
	classTypes_["Heavy"]	= CLASS_HEAVY;
	classTypes_["Engineer"]	= CLASS_ENGINEER;
	classTypes_["Medic"]	= CLASS_MEDIC;
	classTypes_["Sniper"]	= CLASS_SNIPER;
	classTypes_["Spy"]		= CLASS_SPY;

	try {
		// Get definition file.
		string itemDefinition;
		try {
			Curl* curl = Curl::get_instance();
			itemDefinition = curl->read( "http://api.steampowered.com/IEconItems_440/GetSchema/v0001/?key=0270F315C25E569307FEBDB67A497A2E&format=json&language=en" );
		}
		catch (Exception&) {
			throw Exception( "Failed to read item definition file from Steam Web API." );
		}

		// Parse definition file.
		Json::Reader reader;
		if (!reader.parse( itemDefinition, root_, false )) {
			throw Exception( "Failed to parse item definition file." );
		}

		Json::Value& items = root_["result"]["items"];
		if (items == Json::Value::null) {
			throw Exception( "Unexpected definition format, expected result.items." );
		}

		for (Json::ValueIterator i = items.begin(); i != items.end(); ++i) {
			// Check if we're being closed.
			if (stop_) {
				break;
			}

			Json::Value item = *i;

			// Check that all necessary attributes exist.
			unsigned int defindex	= GetMember( item, "defindex" ).asUInt();
			string item_name		= GetMember( item, "item_name" ).asString();
			string image_inventory	= GetMember( item, "image_inventory" ).asString();
			string image_url		= GetMember( item, "image_url" ).asString();

			if (image_inventory.empty()) {
				image_inventory = "backpack/unknown_item";
				image_url = "http://www.jengerer.com/item_manager/img/backpack/unknown_item.png";
			}

			EItemSlot item_slot = SLOT_NONE;
			if (item.isMember( "item_slot" )) {
				string slot_name = GetMember( item, "item_slot" ).asString();
				auto j = slotTypes_.find( slot_name );
				if (j == slotTypes_.end()) {
					throw Exception( "Failed to parse item definitions. Unexpected item slot type '" + slot_name + "' found." );
				}
			}

			// Get classes, if they exist.
			unsigned int item_classes = CLASS_ALL;
			if (item.isMember( "used_by_classes" )) {
				item_classes = CLASS_NONE;
				Json::Value classes = item["used_by_classes"];
				for (size_t i = 0; i < classes.size(); i++) {
					string class_name = classes[i].asString();

					std::map<string, EClassEquip>::iterator k = classTypes_.find( class_name );
					if (k == classTypes_.end()) {
						throw Exception( "Failed to parse item definitions. Unexpected class type '" + class_name + "' found." );
					}

					item_classes |= k->second;
				}
			}

			Texture *texture = graphics_->get_texture( image_inventory, image_url );

			// Generate information object.
			ItemInformation *itemInformation = new ItemInformation(
				item_name,
				texture,
				item_classes,
				item_slot );

			// Parse item type and insert.
			Item::definitions[defindex] = itemInformation;

			// Update progress.
			loaded_++;
			SetProgress( static_cast<float>(loaded_) / static_cast<float>(items.size()) );
		}

		Cleanup();
		SetState( LOADING_STATE_FINISHED );
	}
	catch (Exception& exception) {
		Cleanup();
		SetError( *exception.getMessage() );
	}

	graphics_->unset_render_context();
}

void DefinitionLoader::Cleanup()
{
	root_.clear();
	slotTypes_.clear();
	classTypes_.clear();
}

Json::Value DefinitionLoader::GetMember( Json::Value& root, const string& member )
{
	if (!root.isMember( member )) {
		throw Exception( "Unexpected definition format, key '" + member + "' not found." );
	}

	return root[member];
}

ELoadingState DefinitionLoader::GetState() const
{
	return state_;
}

const string& DefinitionLoader::GetErrorMsg() const
{
	return errorMsg_;
}

float DefinitionLoader::GetProgress() const
{
	return progress_;
}

void DefinitionLoader::SetState( ELoadingState state )
{
	state_ = state;
}

void DefinitionLoader::SetError( const string& errorMsg )
{
	SetProgress( 0.0f );
	SetState( LOADING_STATE_ERROR );
	SetErrorMsg( errorMsg );
}

void DefinitionLoader::SetErrorMsg( const string& errorMsg )
{
	errorMsg_ = errorMsg;
}

void DefinitionLoader::SetProgress( float progress )
{
	progress_ = progress;
}