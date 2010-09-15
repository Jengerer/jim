#pragma once

#include "Exception.h"

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <string>

using namespace std;

/* Struct for downloaded files. */
struct Download {
	const char *cFilename;
	FILE *fStream;
};

/* Struct for reading files. */
struct Read {
	char *cMemory;
	size_t tSize;
};

/* Helper functions for downloading/reading. */
static void *reAllocate(void *vBuffer, size_t tSize);
static size_t fWrite(void *vBuffer, size_t tSize, size_t nMember, void *vStream);
static size_t WriteMemoryCallback(void *vBuffer, size_t tSize, size_t nMember, void *vData);

class Curl
{
public:
	Curl();
	~Curl();

	/* Handling interfaces. */
	void loadInterfaces();
	void closeInterfaces();
	void cleanCurl();

	/* External file functions. */
	bool downloadFile(const string fileURL, const string fileDirectory);
	string readFile(const string fileURL);

private:
	CURL *m_pCurl;
};