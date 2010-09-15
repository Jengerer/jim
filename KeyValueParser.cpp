#include "KeyValueParser.h"

KeyValueParser::KeyValueParser(string parseString)
{
	m_strParse = parseString;
}

KeyValueParser::~KeyValueParser()
{
	/* KeyValueParser has been destroyed. */
}

Hashtable* KeyValueParser::getHashtable()
{
	/* Create the new table. */
	Hashtable* baseTable = new Hashtable();

	/* Keep track of the depth. */
	vector<Hashtable*> tableDepth;

	/* Track which table we're writing to. */
	Hashtable* addingTable = baseTable;
	Hashtable* currentTable = NULL;

	/* String/stream for parsing. */
	string thisLine;
	stringstream parseStream(m_strParse);

	// Parsing index variables.
	int nIndex = 0;
	int nLength = m_strParse.length();

	// Word parsing variables.
	bool writingNow = false;
	string thisWord, lastWord;
	bool hasKey = false;

	int nStart;

	// Begin parsing.
	while (nIndex < nLength)
	{
		switch (m_strParse[nIndex])
		{
		case '/':
			{
				char nextChar = m_strParse[nIndex+1];
				if (nextChar == '/' && !writingNow)
				{
					// This is a commented line, go to next.
					int newLinePos = m_strParse.find('\n', nIndex+1);
					nIndex = newLinePos;
				}
			}
			break;

		case '\\':
			nIndex++;
			break;

		case '"':
			// We've reached the start. Seek the end.
			if (!writingNow)
			{
				nStart = nIndex + 1;
				writingNow = true;
			} else
			{
				// Get the word.
				thisWord = m_strParse.substr(nStart, (nIndex - nStart));
				writingNow = false;
			
				// Check if this is a key..
				if (hasKey)
				{
					string* addWord = new string(thisWord);
					addingTable->put(lastWord, addWord);
					
					// Unset the key.
					hasKey = false;
				} else
				{
					// Just set the key.
					lastWord = thisWord;
					hasKey = true;
				}
			}
			break;

		case '{':
			{
				// Create a new table.
				currentTable = new Hashtable();

				// Add it.
				addingTable->put(lastWord, currentTable);

				// Add to depth vector, and set new adding.
				tableDepth.push_back(addingTable);
				addingTable = currentTable;

				hasKey = false;
			}
			break;

		case '}':
			// Remove last table from depth, and set parent.
			if (tableDepth.empty())
			{
				delete baseTable;
				throw Exception("Unexpected format in KeyValue file. Parsing failed.");
			}

			addingTable = tableDepth.back();
			tableDepth.pop_back();
			break;
		}

		nIndex++;
	}

	return baseTable;
}