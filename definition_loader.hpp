#ifndef DEFINITION_LOADER_HPP
#define DEFINITION_LOADER_HPP

#include <string/constant_string.hpp>
#include <containers/hash_map.hpp>

#include <jui/gfx/graphics_2d.hpp>
#include <boost/thread.hpp>
#include <json/json.h>

#include "resource_loader_interface.hpp"
#include "item.hpp"
#include "item_schema.hpp"
#include "string_hasher.hpp"
#include "notification_queue.hpp"

enum DefinitionLoaderState
{
	LOADING_STATE_NONE,
	LOADING_STATE_START,
	LOADING_STATE_DOWNLOAD_DEFINITIONS,
	LOADING_STATE_LOADING_ATTRIBUTES,
	LOADING_STATE_LOADING_ITEMS,
	LOADING_STATE_ERROR,
	LOADING_STATE_CLEANUP,
	LOADING_STATE_FINISHED
};

// JSON parsing helper function.
bool get_member( Json::Value* root, const JUTIL::String* member );

/*
 * Class for loading item definitions from the Steam Web API.
 */
class DefinitionLoader
{

public:

	DefinitionLoader( JUI::Graphics2D* graphics, ItemSchema* schema, NotificationQueue* notifications );
	~DefinitionLoader( void );

	// Starting and ending the worker threads.
	bool begin( void );
	void end( void );

	// Getting definition loader states.
	DefinitionLoaderState get_state( void ) const;
	float get_progress( void ) const;
	const JUTIL::String* get_progress_message( void );

private:

	bool load( void );
    bool load_definitions( Json::Value* root );
    bool load_item( Json::Value* item,
        JUTIL::DynamicString* name,
        JUTIL::DynamicString* image,
        JUTIL::DynamicString* image_url );
    bool load_item_attribute( Json::Value* attribute, ItemDefinition* information );
	void clean_up( void );

	// Loading state functions.
	bool is_state_changed( void ) const;
	void set_state( DefinitionLoaderState state );

	// Progress counters.
	bool update_progress_message( void );
	void set_progress( size_t loaded, size_t total );
	void set_progress( float percentage );
	bool set_progress_message( const JUTIL::String* progress_msg );

private:

	// Resource parameters.
	JUI::Graphics2D* graphics_;
    ItemSchema* schema_;
	NotificationQueue* notifications_;

	// Threading parameters.
	volatile bool should_stop_;
	boost::thread* thread_;
	boost::mutex mutex_;		

	// Parsing members.
	Json::Value root_;
	JUTIL::HashMap<unsigned int, const JUTIL::String*, EItemSlot, JUTIL::StringHasher> slots_;
	JUTIL::HashMap<unsigned int, const JUTIL::String*, EClassEquip, JUTIL::StringHasher> classes_;
    JUTIL::HashMap<unsigned int, const JUTIL::String*, uint16, JUTIL::StringHasher> name_map_;

	// State members.
	float progress_;
	DefinitionLoaderState state_;
	JUTIL::DynamicString progress_message_;
	bool has_state_changed_;
	
};

#endif // DEFINITION_LOADER_HPP