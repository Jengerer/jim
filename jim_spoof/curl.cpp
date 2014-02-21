#include "curl.hpp"
#include <ctime>

Curl* Curl::instance_ = nullptr;

/*
	* Constructor does nothing.
	*/
Curl::Curl( void )
{
	// Nothing.
}

/*
	* Clean operation and clean up interfaces.
	*/
Curl::~Curl( void )
{
	clean();
	curl_global_cleanup();
}

/*
	* Get an instance; create if necessary.
	*/
Curl* Curl::get_instance( void )
{
	// Initialize instance if not exists.
	if (instance_ == nullptr) {
		instance_ = new Curl();
		instance_->initialize();
	}

	return instance_;
}

/*
 * Delete instance if exists.
 */
void Curl::shut_down( void )
{
	if (instance_ != nullptr) {
		delete instance_;
	}
}

/*
	* Initialize CURL interfaces.
	*/
Curl::ReturnStatus Curl::initialize( void )
{
	// Initialize CURL.
	curl_ = curl_easy_init();
	if (curl_ == nullptr) {
		return InitializeFailure;
	}

	return Success;
}

/*
 * Clean up CURL operations.
 */
void Curl::clean( void )
{
	curl_easy_cleanup( curl_ );
}

/*
 * Read the file at the given URL to string.
 */
bool Curl::read( const std::string* url, std::string* output )
{
	// Set up CURL operation.
	curl_easy_setopt( curl_, CURLOPT_URL, url->c_str() );
	curl_easy_setopt( curl_, CURLOPT_WRITEFUNCTION, write_buffer );
	curl_easy_setopt( curl_, CURLOPT_WRITEDATA, output );
	curl_easy_setopt( curl_, CURLOPT_FAILONERROR, true );
	CURLcode result = curl_easy_perform( curl_ );
	if (result != CURLE_OK) {
		return false;
	}
	return true;
}

/*
 * Download file at given URL to destination.
 */
bool Curl::download( const std::string* url, const std::string* destination )
{
	// Set the URL.
	CURLcode result = curl_easy_setopt( curl_, CURLOPT_URL, url->c_str() );
	if (result != CURLE_OK) {
		return false;
	}

	// Read contents of file.
	std::string contents;
	if (!read( url, &contents )) {
		return false;
	}

	if (!write_to_local_file( destination, &contents )) {
		return false;
	}

	return true;
}


bool Curl::write_to_local_file( const std::string* destination, std::string* input )
{
	// Open file for writing.
	FILE* file;
	errno_t error = fopen_s( &file, destination->c_str(), "wb" );
	if (error != 0) {
		return false;
	}
		
	// Write to file.
	size_t size = input->size();
	size_t written = fwrite( input->c_str(), 1, size, file );
	fclose( file );
	if ( written != size ) {
		return false;
	}
	return true;
}

/*
	* Callback when ready to write.
	*/
static size_t write_buffer( void *buffer, size_t size, size_t num_members, void* data )
{
	// Get real buffer size.
	size_t real_size = size * num_members;
	std::string* string = static_cast<std::string*>(data);

	// Get buffer of proper size.
	size_t old_length = string->size();
	size_t new_length = old_length + real_size;
	string->resize( new_length );

	// Write new string.
	char* write_start = &string->at(old_length);
	memcpy( write_start, buffer, real_size );
	return real_size;
}