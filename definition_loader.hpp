#ifndef DEFINITION_LOADER_HPP
#define DEFINITION_LOADER_HPP

#include <jui/gfx/graphics_2d.hpp>
#include <boost/thread.hpp>
#include <json/json.h>

#include "iresource_loader.hpp"
#include "item.hpp"
#include "item_information.hpp"
#include "string_hasher.hpp"

enum ELoadingState
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

Json::Value& get_member( Json::Value& root, const JUTIL::ConstantString& member );

/*
 * Class for loading item definitions from the Steam Web API.
 */
class DefinitionLoader
{

public:

	DefinitionLoader( JUI::Graphics2D* graphics );
	~DefinitionLoader();

	// Starting and ending the worker threads.
	void begin( void );
	void end( void );

	// Getting definition loader states.
	ELoadingState get_state( void ) const;
	float get_progress( void ) const;
	void update_progress_msg( void );
	const JUTIL::ConstantString* get_progress_msg( void ) const;

private:

	void load();
	void clean_up();

	// Loading state functions.
	bool is_state_changed() const;
	void set_state( ELoadingState state );
	void set_error( const std::string& error_msg );

	// Progress counters.
	void set_progress( size_t loaded, size_t total );
	void set_progress( float percentage );
	void set_progress_msg( const std::string& progress_msg );

private:

	// Resource parameters.
	JUI::Graphics2D* graphics_;

	// Threading parameters.
	volatile bool should_stop_;
	boost::shared_ptr<boost::thread> thread_;
	boost::mutex mutex_;		

	// Parsing members.
	Json::Value root_;
	std::hash_map<std::string, EItemSlot, StringHasher> slots_;
	std::hash_map<std::string, EClassEquip, StringHasher> classes_;

	// State members.
	float progress_;
	std::string progress_msg_;
	std::string error_msg_;
	ELoadingState state_;
	bool has_state_changed_;
	
};

#endif // DEFINITION_LOADER_HPP