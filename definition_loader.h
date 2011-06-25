#ifndef DEFINITION_LOADER_H
#define DEFINITION_LOADER_H

#include "directx.h"
#include "item.h"
#include "item_information.h"

#include <string>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

enum ELoadingState
{
	LOADING_STATE_START,
	LOADING_STATE_RUNNING,
	LOADING_STATE_ERROR,
	LOADING_STATE_FINISHED
};

class DefinitionLoader
{

public:

	DefinitionLoader( DirectX *directX, const string& definitionUrl );

	// Starting and ending the worker threads.
	void Begin();
	void End();

	// Getting definition loader states.
	ELoadingState GetState() const;
	const string& GetErrorMsg() const;

private:

	void Load();
	void SetState( ELoadingState state );
	void SetError( const string& errorMsg );

private:

	// Resource parameters.
	DirectX*		directX_;
	string			definitionUrl_;

	// Threading parameters.
	volatile bool						stop_;
	boost::shared_ptr<boost::thread>	thread_;
	boost::mutex						mutex_;

	// State members.
	ELoadingState						state_;
	string								errorMsg_;
	
};

#endif // DEFINITION_LOADER_H