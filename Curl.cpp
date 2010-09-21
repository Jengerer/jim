#include "Curl.h"

Curl::Curl()
{
	// Curl object has been created.
	try
	{
		loadInterfaces();
	} catch (Exception curlException)
	{
		closeInterfaces();
		throw curlException;
	}
}

Curl::~Curl()
{
	// Curn object has been destroyed.
	closeInterfaces();
}

void Curl::loadInterfaces()
{
	m_pCurl = curl_easy_init();
	if (m_pCurl == NULL)
		throw Exception("Failed to initialize cURL.");
}

void Curl::closeInterfaces()
{
	cleanCurl();
	curl_global_cleanup();
}

void Curl::cleanCurl()
{
	curl_easy_cleanup(m_pCurl);
}

bool Curl::downloadFile(const string& fileURL, const string& fileDirectory)
{
	//Set the URL.
	CURLcode cRes = curl_easy_setopt(m_pCurl, CURLOPT_URL, fileURL.c_str());

	if (cRes != CURLE_OK)
	{
		throw Exception("Failed to set cURL options.");
		return false;
	}

	// Create the folder(s) if needed.
	size_t nextSlash = fileDirectory.find('/');
	while (nextSlash != string::npos)
	{
		string currentPath = fileDirectory.substr(0, nextSlash);
		if (GetFileAttributes(currentPath.c_str()) == INVALID_FILE_ATTRIBUTES)
			CreateDirectory(currentPath.c_str(), NULL);

		nextSlash = fileDirectory.find("/", nextSlash+1);
	}

	// Create the file.
	struct Download downloadFile = {
		fileDirectory.c_str(),
		NULL
	};

	// Get the file ready for downloading.
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void *)&downloadFile);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, fWrite);
	curl_easy_setopt(m_pCurl, CURLOPT_FAILONERROR, true);

	// Get it!
	cRes = curl_easy_perform(m_pCurl);

	// Close the stream if it exists.
	if (downloadFile.fStream)
		fclose(downloadFile.fStream);

	if (cRes != CURLE_OK) return false;

	return true;
}

string Curl::readFile(const string& fileURL)
{
	struct Read readFile;
	readFile.cMemory = NULL;
	readFile.tSize = 0;

	//Specify url.
	curl_easy_setopt(m_pCurl, CURLOPT_URL, fileURL.c_str());

	//Send all data to this function.
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	//Send chuck struct.
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void*)&readFile);

	//Get it!
	CURLcode cRes = curl_easy_perform(m_pCurl);
	if (cRes != CURLE_OK)
		throw Exception("Failed to download file.");

	//Now get the string.
	string thisString = readFile.cMemory;

	//Free the memory.
	if (readFile.cMemory)
		free(readFile.cMemory);

	return thisString;
}

static void *reAllocate(void *vBuffer, size_t tSize)
{
	if (vBuffer)
		return realloc(vBuffer, tSize);
	else
		return malloc(tSize);
}

static size_t WriteMemoryCallback(void *vBuffer, size_t tSize, size_t nMember, void *vData)
{
	size_t realSize = tSize*nMember;
	struct Read *readFile = (struct Read *)vData;

	readFile->cMemory = (char*)reAllocate(readFile->cMemory, readFile->tSize + realSize + 1);
	if (readFile->cMemory) {
		memcpy(&(readFile->cMemory[readFile->tSize]), vBuffer, realSize);
		readFile->tSize += realSize;
		readFile->cMemory[readFile->tSize] = 0;
	}

	return realSize;
}

static size_t fWrite(void *vBuffer, size_t tSize, size_t nMember, void *vStream)
{
	struct Download *downloadFile = (struct Download *)vStream;

	if (downloadFile && !downloadFile->fStream)
	{
		fopen_s(&downloadFile->fStream, downloadFile->cFilename, "wb");		
		if (!downloadFile->fStream)
			return -1;
	}

	return fwrite(vBuffer, tSize, nMember, downloadFile->fStream);
}