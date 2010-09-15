#include "Parser.h"

#include <string>
#include <hash_map>
#include <iostream>

using namespace std;

Hashtable *itemDefinitions = NULL, *langDefinitions = NULL;

void loadDefinitions();
void exitApplication();

int main()
{
	try
	{
		loadDefinitions();
	} catch (Exception defineException)
	{
		cout << defineException.getMessage() << endl;
	}

	exitApplication();

	// Pause for confirmation.
	system("PAUSE");

	return 0;
}

void loadDefinitions()
{
	cout << "Starting item definition parser." << endl;

	Curl* pCurl = new Curl();

	string itemDefinition, langDefinition;
	
	// Download API definitions.
	cout << "Opening item definitions... ";
	itemDefinition = pCurl->readFile("http://api.steampowered.com/ITFItems_440/GetSchema/v0001/?key=0270F315C25E569307FEBDB67A497A2E&format=vdf");
	cout << "loaded!" << endl;

	// Download language definitions.
	cout << "Opening language definitions... ";
	langDefinition = pCurl->readFile("http://www.jengerer.com/tf_english.txt");
	cout << "loaded!" << endl;

	// Spacing!
	cout << endl;

	KeyValueParser definitionParser(itemDefinition);
	KeyValueParser languageParser(langDefinition);

	try
	{
		cout << "Parsing item definitions... ";
		itemDefinitions = definitionParser.getHashtable();
		cout << "done!" << endl;

		cout << "Parsing language definitions... ";
		langDefinitions = languageParser.getHashtable();
		cout << "done!" << endl;
	} catch (Exception parseException)
	{
		throw Exception("Failed to parse items and language: " + parseException.getMessage());
	}

	Hashtable *resultTable, *itemsTable, *definitionsTable;
	Hashtable *langTable, *tokenTable;

	try
	{
		resultTable = itemDefinitions->getTable("result");
		itemsTable = resultTable->getTable("items");
		definitionsTable = itemsTable->getTable("item");
	} catch (Exception tableException)
	{
		throw Exception("Unexpected format for item definitions. Key '" + tableException.getMessage() + "' not found.");
	}

	try
	{
		langTable = langDefinitions->getTable("lang");
		tokenTable = langTable->getTable("Tokens");
	} catch (Exception tableException)
	{
		throw Exception("Unexpected format for language definitions. Key '" + tableException.getMessage() + "' not found.");
	}

	Json::Value jsonRoot;
	Json::StyledWriter jsonWriter;

	/* Load all textures for items. */
	stringHashMap::iterator hashIterator, itemIterator, nameIterator;
	for (hashIterator = definitionsTable->begin(); hashIterator != definitionsTable->end(); hashIterator++)
	{
		Hashtable* thisTable = (Hashtable*)hashIterator->second;

		// Try get the item name.
		string *itemName, *realName, *itemIndex, *itemSlot, *texturePath, *textureURL;
		try
		{
			itemName = thisTable->getString("item_name");
			itemIndex = thisTable->getString("defindex");
			itemSlot = thisTable->getString("item_slot");
			texturePath = thisTable->getString("image_inventory");
			textureURL = thisTable->getString("image_url");
		} catch (Exception tableException)
		{
			throw Exception("Unexpected format for item definitions. Found item with no 'item_name' value.");
		}

		boost::regex langToken("#(.*)");
		boost::cmatch regexMatches;
		if (!boost::regex_match(itemName->c_str(), regexMatches, langToken))
			throw Exception("Unexpected format for item definitions. Tokened name didn't match token format.");

		*itemName = regexMatches[1];

		try
		{
			realName = tokenTable->getString(*itemName);
		} catch (Exception tableException)
		{
			throw Exception("Couldn't find language translation of '" + tableException.getMessage() + "'.");
		}

		Json::Value thisObject;
		thisObject["itemName"] = *realName;
		thisObject["itemSlot"] = *itemSlot;
		thisObject["imageInventory"] = *texturePath;
		thisObject["imageURL"] = *textureURL;

		Json::Value allClasses;
		allClasses.append("scout");
		allClasses.append("soldier");
		allClasses.append("pyro");
		allClasses.append("demoman");
		allClasses.append("heavy");
		allClasses.append("engineer");
		allClasses.append("medic");
		allClasses.append("sniper");
		allClasses.append("spy");

		Hashtable *usedByTable = NULL, *classTable = NULL;
		try
		{
			usedByTable = thisTable->getTable("used_by_classes");
		} catch (Exception tableException)
		{
			// Item is used by all classes.
			thisObject["itemClasses"] = allClasses;
		}

		if (usedByTable != NULL)
		{
			try
			{
				classTable = usedByTable->getTable("class");

				if (!classTable->empty())
				{
					Json::Value itemClasses;
					stringHashMap::iterator hashIterator;
					for (hashIterator = classTable->begin(); hashIterator != classTable->end(); hashIterator++)
					{
						// Get the class name.
						StringObject* classNameObj = (StringObject*)hashIterator->second;
						string* className = classNameObj->getString();
						*className = toLower(*className);
						itemClasses.append(*className);
					}

					thisObject["itemClasses"] = itemClasses;
				}
			} catch (Exception classException)
			{
				throw Exception("Unexpected definition format. " + *realName + " has class usage, but no classes defined.");
			}			
		}
		jsonRoot[*itemIndex] = thisObject;
	}

	ofstream jsonOutput("itemDefinitions.json");
	if (!jsonOutput)
		throw Exception("Couldn't write to file.");

	string jsonText = jsonWriter.write(jsonRoot);
	jsonOutput << jsonText;
	jsonOutput.close();

	cout << "Wrote to file successfully." << endl;
}

void exitApplication()
{
	// Erase all tables.
	delete itemDefinitions;
	delete langDefinitions;
	itemDefinitions = NULL;
	langDefinitions = NULL;
}