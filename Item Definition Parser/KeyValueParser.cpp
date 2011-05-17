#include "KeyValueParser.h"

#include <stack>

Hashtable* KeyValueParser::Parse( const string& inputString )
{
	// Create base map.
	Hashtable *baseTable = new Hashtable();
	stack<Hashtable*> tableStack;

	// Track where we're writing to.
	Hashtable* addingTable = baseTable;
	Hashtable* currentTable = nullptr;

	// Parsing index variables.
	int index = 0;
	int length = inputString.length();

	// Word parsing variables.
	bool writingNow = false;
	string thisWord, lastWord;
	bool hasKey = false;

	int start;

	// Begin parsing.
	while (index < length)
	{
		switch (inputString[index])
		{
		case '/':
			{
				char nextChar = inputString[index+1];
				if (nextChar == '/' && !writingNow)
				{
					// This is a commented line, go to next.
					int nextLine = inputString.find( '\n', index+1 );
					index = nextLine;
				}
			}
			break;

		case '\\':
			index++;
			break;

		case '"':
			// Start of quoted value.
			if (!writingNow)
			{
				start = index + 1;
				writingNow = true;
			} else
			{
				// Get the word.
				thisWord = inputString.substr( start, (index - start) );
				writingNow = false;
			
				// Check if this is a key..
				if (hasKey) {
					string* addWord = new string( thisWord );
					addingTable->put( lastWord, addWord );
				}
				else {
					// Just set the key.
					lastWord = thisWord;
				}

				hasKey = !hasKey;
			}
			break;

		case '{':
			{
				// Create a new table.
				currentTable = new Hashtable();

				// Add it.
				addingTable->put( lastWord, currentTable );

				// Add to depth vector, and set new adding.
				tableStack.push( addingTable );
				addingTable = currentTable;

				hasKey = false;
			}
			break;

		case '}':
			// Remove last table from depth, and set parent.
			if (tableStack.empty()) {
				delete baseTable;
				throw Exception( "Unexpected format in KeyValue file. Parsing failed." );
			}

			addingTable = tableStack.top();
			tableStack.pop();
			break;
		}

		index++;
	}

	return baseTable;
}