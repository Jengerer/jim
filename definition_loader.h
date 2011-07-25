#ifndef DEFINITION_LOADER_H
#define DEFINITION_LOADER_H

#include "graphics_2d.h"
#include "item.h"
#include "item_information.h"

#include <string>

#include <json/json.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

enum ELoadingState
{
	LOADING_STATE_START,
	LOADING_STATE_RUNNING,
	LOADING_STATE_ERROR,
	LOADING_STATE_CLEANUP,
	LOADING_STATE_FINISHED
};

class DefinitionLoader
{

public:

	DefinitionLoader( Graphics2D* graphics, const string& definitionUrl );

	// Starting and ending the worker threads.
	void Begin();
	void End();

	// Getting definition loader states.
	ELoadingState GetState() const;
	const string& GetErrorMsg() const;
	float GetProgress() const;

private:

	void Load();
	void Cleanup();

	Json::Value GetMember( Json::Value& root, const string& member );

	void SetState( ELoadingState state );
	void SetError( const string& errorMsg );
	void SetErrorMsg( const string& errorMsg );
	void SetProgress( float progress );

private:

	// Resource parameters.
	Graphics2D*		graphics_;
	string			definitionUrl_;

	// Threading parameters.
	volatile bool						stop_;
	boost::shared_ptr<boost::thread>	thread_;
	boost::mutex						mutex_;

	// Parsing members.
	Json::Value							root_;
	std::map<string, EItemSlot>			slotTypes_;
	std::map<string, EClassEquip>		classTypes_;

	// State members.
	size_t								loaded_;
	float								progress_;
	ELoadingState						state_;
	string								errorMsg_;
	
};

#endif // DEFINITION_LOADER_H