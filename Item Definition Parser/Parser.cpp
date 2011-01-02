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
		cout << "Exception: " << defineException.getMessage() << endl;
	}

	exitApplication();

	// Pause for confirmation.
	system("PAUSE");

	return 0;
}

void loadDefinitions()
{
	cout << "Starting item definition parser." << endl;

	Curl* curl = new Curl();

	string itemDefinition, langDefinition;
	
	// Download API definitions.
	cout << "Opening item definitions... ";
	itemDefinition = curl->read( "http://api.steampowered.com/ITFItems_440/GetSchema/v0001/?key=0270F315C25E569307FEBDB67A497A2E&format=vdf" );
	cout << "loaded!" << endl;

	// Download language definitions.
	cout << "Opening language definitions... ";
	langDefinition = curl->read( "http://www.jengerer.com/itemManager/tf_english.txt" );
	cout << "loaded!" << endl;

	// Spacing!
	cout << endl;

	KeyValueParser definitionParser( itemDefinition );
	KeyValueParser languageParser( langDefinition );

	try {
		cout << "Parsing item definitions... ";
		itemDefinitions = definitionParser.getHashtable();
		cout << "done!" << endl;

		cout << "Parsing language definitions... ";
		langDefinitions = languageParser.getHashtable();
		cout << "done!" << endl;
	}
	catch (Exception parseException) {
		throw Exception("Failed to parse items and language: " + *parseException.getMessage());
	}

	Hashtable *resultTable, *itemsTable, *definitionsTable;
	Hashtable *langTable, *tokenTable;

	try {
		resultTable = itemDefinitions->getTable( "result");
		itemsTable = resultTable->getTable( "items" );
		definitionsTable = itemsTable->getTable( "item" );
	}
	catch (Exception tableException) {
		throw Exception("Unexpected format for item definitions. Key '" + *tableException.getMessage() + "' not found.");
	}

	try {
		langTable = langDefinitions->getTable("lang");
		tokenTable = langTable->getTable("Tokens");
	}
	catch (Exception tableException) {
		throw Exception("Unexpected format for language definitions. Key '" + *tableException.getMessage() + "' not found.");
	}

	Json::Value root;
	Json::StyledWriter jsonWriter;

	ofstream defLegacy;
	defLegacy.open("itemDefinitions.txt");

	// Handle items.
	stringMap::iterator hashIterator, itemIterator, nameIterator;
	for (hashIterator = definitionsTable->begin(); hashIterator != definitionsTable->end(); hashIterator++)
	{
		Hashtable* thisTable = NULL;
		try
		{
			thisTable = boost::any_cast<Hashtable*>(hashIterator->second);
		} catch (const boost::bad_any_cast &)
		{
			throw Exception( "Failed to get table, unexpected variable type received." );
		}

		// Try get the item name.
		string *itemName, *realName, *itemIndex, *itemSlot, *texturePath, *textureUrl;
		try
		{
			itemName = thisTable->getString("item_name");
			itemIndex = thisTable->getString("defindex");
			itemSlot = thisTable->getString("item_slot");
			texturePath = thisTable->getString("image_inventory");
			textureUrl = thisTable->getString("image_url");
		} catch (Exception tableException)
		{
			throw Exception("Unexpected format for item definitions. Found item with no '" + *tableException.getMessage() + "' value.");
		}

		if (texturePath->empty()) {
			*texturePath = "backpack/unknown_item";
			*textureUrl = "http://www.jengerer.com/itemManager/imgFiles/backpack/unknown_item.png";
		}

		if (!curl->download(*textureUrl, "imgFiles/" + *texturePath + ".png")) {
			*texturePath = "backpack/unknown_item";
		}

		boost::regex langToken("#(.*)");
		boost::cmatch regexMatches;
		if (!boost::regex_match(itemName->c_str(), regexMatches, langToken))
			throw Exception( "Unexpected format for item definitions. Tokened name didn't match token format." );

		*itemName = regexMatches[1];

		try
		{
			realName = tokenTable->getString(*itemName);
		} catch (Exception& tableException)
		{
			printf("Couldn't find language translation of item %s name '%s'.\n", itemIndex->c_str(), tableException.getMessage()->c_str());
			realName = itemName;
		}

		Json::Value thisObject;
		thisObject["index"] = *itemIndex;
		thisObject["name"] = *realName;
		thisObject["slot"] = *itemSlot;
		thisObject["image"] = *texturePath;

		string slotName = lower(*itemSlot);

		int slot = 0;
		if (slotName == "primary")
			slot = 1;
		else if (slotName == "secondary")
			slot = 2;
		else if (slotName == "melee")
			slot = 3;
		else if (slotName == "pda")
			slot = 4;
		else if (slotName == "pda2")
			slot = 5;
		else if (slotName == "building")
			slot = 6;
		else if (slotName == "head")
			slot = 7;
		else if (slotName == "misc")
			slot = 8;
		else if (slotName == "action")
			slot = 9;
		else if (slotName == "engineer")
			slot = 10;
		else
			throw Exception("Unknown slot: " + *itemSlot);

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
			thisObject["classes"] = allClasses;
		}

		int classIndex = 0;

		if (usedByTable != NULL)
		{
			try
			{
				classTable = usedByTable->getTable("class");
			} catch (Exception classException)
			{
				throw Exception("Unexpected definition format. " + *realName + " has class usage, but no classes defined.");
			}

			if (!classTable->empty())
			{
				if (classTable->size() == 1) {
					stringMap::iterator iter = classTable->begin();
					string* name = boost::any_cast<string*>(iter->second);
					string className = lower(*name);
					if (className == "scout")
						classIndex = 1;
					else if (className == "soldier")
						classIndex = 2;
					else if (className == "pyro")
						classIndex = 3;
					else if (className == "demoman")
						classIndex = 4;
					else if (className == "heavy")
						classIndex = 5;
					else if (className == "engineer")
						classIndex = 6;
					else if (className == "medic")
						classIndex = 7;
					else if (className == "sniper")
						classIndex = 8;
					else if (className == "spy")
						classIndex = 9;
					else {
						throw Exception("Unknown class name: " + className);
					}
				}

				Json::Value itemClasses;
				stringMap::iterator hashIterator;
				for (hashIterator = classTable->begin(); hashIterator != classTable->end(); hashIterator++)
				{
					// Get the class name.
					try
					{
						string* className = boost::any_cast<string*>(hashIterator->second);
						*className = lower(*className);
						itemClasses.append(*className);
					} catch (const boost::bad_any_cast &)
					{
						throw Exception( "Bad cast for class name. Expected string." );
					}
				}

				thisObject["classes"] = itemClasses;
			}
		}

		defLegacy << *itemIndex << " " << *realName << "; " << *texturePath << " " << classIndex << " " << slot << "\n";
		root.append(thisObject);
	}

	// Add definition for unknown item.
	Json::Value unknownItem;
	unknownItem["index"] = "-1";
	unknownItem["name"] = "Unknown Item";
	unknownItem["slot"] = "misc";
	unknownItem["image"] = "backpack/unknown_item";

	defLegacy << "-1 Unknown Item; backpack/unknown_item 0 0";
	defLegacy.close();

	// Set -1 to be unknown index.
	root.append(unknownItem);

	ofstream jsonOutput("itemDefinitions.json");
	if (!jsonOutput)
		throw Exception( "Couldn't write to file." );

	string jsonText = jsonWriter.write(root);
	jsonOutput << jsonText;
	jsonOutput.close();

	cout << "Wrote to file successfully." << endl;
}

void exitApplication()
{
	cout << "Deallocating memory stored in tables... ";
	// Erase all tables.
	delete itemDefinitions;
	delete langDefinitions;

	itemDefinitions = NULL;
	langDefinitions = NULL;
	cout << "done!" << endl;
}