#include "definition_loader.h"

#include <algorithm>
#include <json/json.h>

DefinitionLoader::DefinitionLoader( DirectX *directX, const string& definitionUrl )
{
	stop_ = false;
	directX_ = directX;
	definitionUrl_ = definitionUrl;
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
	try {
		// Load the item definitions.
		string itemDefinitions;
		try {
			itemDefinitions = directX_->read( definitionUrl_ );
		}
		catch (Exception&) {
			throw Exception( "Failed to retrieve item definitions from server." );
		}

		// Begin parsing.
		Json::Value	root;
		Json::Reader reader;
		if (!reader.parse( itemDefinitions, root, false )) {
			throw Exception("Failed to parse item definitions.");
		}

		if (!root.isMember( "items" )) {
			throw Exception( "Unexpected definition format. Element 'items' not found." );
		}

		Json::Value items = root.get( "items", root );
		for (Json::ValueIterator i = items.begin(); i != items.end(); ++i) {
			if (!stop_) {
				Json::Value thisItem = *i;

				// TODO: Put member_not_found exception and catch specific name.
				if (!(thisItem.isMember("index") && thisItem.isMember("name") &&
					thisItem.isMember("slot") && thisItem.isMember("image"))) {
					throw Exception("Failed to parse item definitions. One or more missing members from item entry.");
				}

				// Get strings.
				uint32 index	= thisItem.get( "index", root ).asUInt();
				string name		= thisItem.get( "name", root ).asString();
				string image	= thisItem.get( "image", root ).asString();
				EItemSlot slot	= static_cast<EItemSlot>(thisItem.get( "slot", root ).asUInt());
				uint32 classes	= thisItem.get( "classes", root ).asUInt();

				// Lock before we check and load.
				boost::mutex::scoped_lock lock( mutex_ );

				// Don't reload.
				information_map::iterator j = Item::definitions.find( index );
				if (j != Item::definitions.end()) {
					continue;
				}

				// Attempt to load the texture.
				Texture *texture = nullptr;
				try {
					texture = directX_->GetTexture( image );
				}
				catch (Exception &textureException) {
					throw textureException;
				}

				// Generate information object.
				ItemInformation *itemInformation = new ItemInformation(
					name,
					texture,
					classes,
					slot );

				// Parse item type and insert.
				Item::definitions[index] = itemInformation;
			}
		}
		
		SetState( LOADING_STATE_FINISHED );
	}
	catch (Exception& exception) {
		SetError( *exception.getMessage() );
	}
}

ELoadingState DefinitionLoader::GetState() const
{
	return state_;
}

const string& DefinitionLoader::GetErrorMsg() const
{
	return errorMsg_;
}

void DefinitionLoader::SetState( ELoadingState state )
{
	state_ = state;
}

void DefinitionLoader::SetError( const string& errorMsg )
{
	SetState( LOADING_STATE_ERROR );
	errorMsg_ = errorMsg;
}