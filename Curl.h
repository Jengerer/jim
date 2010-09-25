#pragma once

#include "Exception.h"

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <string>

using namespace std;

/* Struct for downloaded files. */
struct Download_t {
	const char* filename;
	FILE* file;
};

/* Struct for reading files. */
struct Memory_t {
	char* memory;
	size_t size;
};

/* Helper functions for downloading/reading. */
static void *reallocate( void *buffer, size_t size );
static size_t write( void *buffer, size_t size, size_t nMembers, void* data );
static size_t writeCallback( void *buffer, size_t size, size_t nMembers, void* data );

class Curl
{
public:
	Curl();
	~Curl();

	// Handling interfaces.
	void	openInterfaces();
	void	closeInterfaces();
	void	clean();

	// External file functions.
	bool	download( const string& url, const string& destination );
	string	read( const string& url );

private:
	CURL*	curl_;
};