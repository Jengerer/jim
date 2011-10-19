#pragma once

#include <stdexcept>

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

	static Curl* get_instance();
	static void shut_down();

	// External file functions.
	void	download( const std::string& url, const std::string& destination );
	string	read( const std::string& url );

private:

	Curl();
	virtual ~Curl();

	// Handling interfaces.
	void	initialize();
	void	close();
	void	clean();

private:
	
	static Curl* instance_;
	static CURL* curl_;

};