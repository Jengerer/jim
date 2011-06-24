#include "curl.h"

Curl::Curl()
{
	// Curl object has been created.
	try
	{
		openInterfaces();
	} catch ( Exception exception )
	{
		closeInterfaces();
		throw exception;
	}
}

Curl::~Curl()
{
	// Curn object has been destroyed.
	closeInterfaces();
}

void Curl::openInterfaces()
{
	curl_ = curl_easy_init();
	if (curl_ == NULL)
		throw Exception( "Failed to initialize cURL." );
}

void Curl::closeInterfaces()
{
	clean();
	curl_global_cleanup();
}

void Curl::clean()
{
	curl_easy_cleanup( curl_ );
}

bool Curl::download( const string& url, const string& destination )
{
	//Set the URL.
	CURLcode result = curl_easy_setopt( curl_, CURLOPT_URL, url.c_str() );

	if (result != CURLE_OK)
		return false;

	// Create the folder(s) if needed.
	size_t slash = destination.find( '/' );
	while (slash != string::npos) {
		string path = destination.substr( 0, slash );
		if ( GetFileAttributes( path.c_str() ) == INVALID_FILE_ATTRIBUTES )
			CreateDirectory( path.c_str(), 0 );

		slash = destination.find( "/", slash+1 );
	}

	// Create the file.
	Download_t downloadFile = {
		destination.c_str(),
		0
	};

	// Get the file ready for downloading.
	curl_easy_setopt( curl_, CURLOPT_WRITEDATA, &downloadFile );
	curl_easy_setopt( curl_, CURLOPT_WRITEFUNCTION, write );
	curl_easy_setopt( curl_, CURLOPT_FAILONERROR, true );

	// Get it!
	result = curl_easy_perform( curl_ );
	if (result != CURLE_OK)
		return false;

	// Close the stream if it exists.
	if ( downloadFile.file )
		fclose( downloadFile.file );

	// All succeeded.
	return true;
}

string Curl::read( const string& url )
{
	Memory_t readFile;
	ZeroMemory( &readFile, sizeof(Memory_t) );

	//Specify url.
	curl_easy_setopt( curl_, CURLOPT_URL, url.c_str() );

	//Send all data to this function.
	curl_easy_setopt( curl_, CURLOPT_WRITEFUNCTION, writeCallback );

	//Send chuck struct.
	curl_easy_setopt( curl_, CURLOPT_WRITEDATA, &readFile );

	// Fail if not found.
	curl_easy_setopt( curl_, CURLOPT_FAILONERROR, true );

	//Get it!
	CURLcode result = curl_easy_perform( curl_ );
	if (result != CURLE_OK)
		throw Exception( "Failed to read contents of page." );

	//Now get the string.
	string output = readFile.memory;

	//Free the memory.
	if (readFile.memory != 0)
		free( readFile.memory );

	return output;
}

static void *reallocate( void *buffer, size_t size )
{
	return (buffer != 0 ? realloc( buffer, size) : malloc( size ));
}

static size_t writeCallback( void *buffer, size_t size, size_t nMembers, void* data )
{
	size_t realSize = size*nMembers;
	struct Memory_t *readFile = (struct Memory_t *)data;

	readFile->memory = (char*)reallocate( readFile->memory, readFile->size + realSize + 1 );
	if (readFile->memory != 0) {
		void* copyMemory = &readFile->memory[readFile->size];
		memcpy( copyMemory, buffer, realSize );
		readFile->size += realSize;
		readFile->memory[readFile->size] = 0;
	}

	return realSize;
}

static size_t write( void *buffer, size_t size, size_t nMembers, void* data )
{
	struct Download_t *downloadFile = (struct Download_t *)data;

	if ((downloadFile != 0) && (downloadFile->file == 0)) {
		fopen_s( &downloadFile->file, downloadFile->filename, "wb" );
		if (downloadFile->file == 0)
			return -1;
	}

	return fwrite( buffer, size, nMembers, downloadFile->file );
}