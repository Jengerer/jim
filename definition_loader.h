#ifndef DEFINITION_LOADER_H
#define DEFINITION_LOADER_H

#include "graphics_2d.h"
#include "item.h"
#include "item_information.h"
#include "string_hasher.h"

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <json/json.h>
#include <sstream>
#include <string>

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

Json::Value get_member( Json::Value& root, const string& member );

class DefinitionLoader
{

public:

	DefinitionLoader( Graphics2D* graphics );
	~DefinitionLoader();

	// Starting and ending the worker threads.
	void begin();
	void end();

	// Getting definition loader states.
	ELoadingState get_state() const;
	float get_progress() const;
	void update_progress_msg();
	const string& get_progress_msg() const;

private:

	void load();
	void clean_up();

	// Loading state functions.
	bool is_state_changed() const;
	void set_state( ELoadingState state );
	void set_error( const string& error_msg );

	// Progress counters.
	void set_progress( size_t loaded, size_t total );
	void set_progress( float percentage );
	void set_progress_msg( const string& progress_msg );

private:

	// Resource parameters.
	Graphics2D*		graphics_;

	// Threading parameters.
	volatile bool						stop_;
	boost::shared_ptr<boost::thread>	thread_;
	boost::mutex						mutex_;		

	// Parsing members.
	Json::Value root_;
	std::hash_map<string, EItemSlot, StringHasher> slots_;
	std::hash_map<string, EClassEquip, StringHasher> classes_;

	// State members.
	float								progress_;
	string								progress_msg_;
	string								error_msg_;
	ELoadingState						state_;
	bool								state_changed_;
	
};

#endif // DEFINITION_LOADER_H