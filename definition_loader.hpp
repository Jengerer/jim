#ifndef DEFINITION_LOADER_HPP
#define DEFINITION_LOADER_HPP

#include <string/constant_string.hpp>
#include <containers/hash_map.hpp>

#include <jui/gfx/graphics_2d.hpp>
#include <thread>
#include <mutex>
#include <json/json.h>

#include "resource_loader_interface.hpp"
#include "item.hpp"
#include "item_schema.hpp"
#include "notification_queue.hpp"
#include "vdf_parser.hpp"

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
class DefinitionLoader : public VdfVisitor
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

	// VDF visitor methods
	virtual void on_object_begin( const JUTIL::ConstantString& name ) override;
	virtual void on_object_end() override;
	virtual void on_property( const JUTIL::ConstantString& name, const JUTIL::ConstantString& value ) override;

private:

	// Top-level definition parsing method
	bool load( void );

	// Types for callbacks
	typedef bool (DefinitionLoader::*ObjectBeginHandler)( const JUTIL::ConstantString& );
	typedef bool (DefinitionLoader::*ObjectEndHandler)( void );
	typedef bool (DefinitionLoader::*PropertyHandler)( const JUTIL::ConstantString&, const JUTIL::ConstantString& );

	// Helper methods for visitor
	bool push_handlers( ObjectBeginHandler begin_handler, ObjectEndHandler end_handler, PropertyHandler property_handler );
	bool pop_handlers();

	// Default handlers for unimportant fields
	bool on_default_object_begin( const JUTIL::ConstantString& object );
	bool on_default_object_end();
	bool on_default_property( const JUTIL::ConstantString& key, const JUTIL::ConstantString& value );
	bool on_error_object_begin( const JUTIL::ConstantString& object );
	bool on_error_object_end();
	bool on_error_property( const JUTIL::ConstantString& key, const JUTIL::ConstantString& value );

	// Object begin methods
	bool on_root_object_begin( const JUTIL::ConstantString& object );
	bool on_root_object_end();
	bool on_result_object_begin( const JUTIL::ConstantString& object );
	bool on_item_begin( const JUTIL::ConstantString& index );
	bool on_item_end();
	bool on_item_subobject_begin( const JUTIL::ConstantString& name );
	bool on_item_attribute_begin( const JUTIL::ConstantString& index );
	bool on_item_attribute_end();
	bool on_item_tool_begin( const JUTIL::ConstantString& name );

	// Property methods
	bool on_quality_name_property( const JUTIL::ConstantString& quality_key, const JUTIL::ConstantString& quality_value );
	bool on_origin_name_property( const JUTIL::ConstantString& key, const JUTIL::ConstantString& value );
	bool on_item_property( const JUTIL::ConstantString& key, const JUTIL::ConstantString& value );
	bool on_attribute_property( const JUTIL::ConstantString& key, const JUTIL::ConstantString& value );
	bool on_tool_property( const JUTIL::ConstantString& key, const JUTIL::ConstantString& value );

    bool load_definitions( Json::Value* root );
	bool load_attributes( Json::Value* result );
	bool load_qualities( Json::Value* result );
	bool load_origins( Json::Value* result );
	bool load_kill_eater_ranks( Json::Value* result );
	bool load_kill_eater_types( Json::Value* result );
	bool load_items( Json::Value* result );
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
	std::thread* thread_;
	std::mutex mutex_;		

	// Parsing members.
	JUTIL::Vector<ObjectBeginHandler> object_begin_stack_;
	JUTIL::Vector<ObjectEndHandler> object_end_stack_;
	JUTIL::Vector<PropertyHandler> property_stack_;
	JUTIL::HashMap<unsigned int, const JUTIL::String*, EInventoryClass, JUTIL::StringHasher> classes_;
	JUTIL::HashMap<unsigned int, const JUTIL::String*, EItemToolType, JUTIL::StringHasher> tools_;
    JUTIL::HashMap<unsigned int, const JUTIL::String*, uint16, JUTIL::StringHasher> name_map_;

	// Temporary objects for parsing
	ItemDefinition* current_item_;

	// State members.
	float progress_;
	DefinitionLoaderState state_;
	JUTIL::DynamicString progress_message_;
	bool has_state_changed_;
	
};

#endif // DEFINITION_LOADER_HPP