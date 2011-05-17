#include "Parser.h"

#include <string>
#include <iostream>
#include <json/json.h>

using namespace std;

Hashtable *itemDefinitions = NULL, *langDefinitions = NULL;

void loadDefinitions();
void exitApplication();

int main()
{
	try {
		loadDefinitions();
	}
	catch (Exception defineException) {
		cout << "Exception: " << *defineException.getMessage() << endl;
	}

	exitApplication();

	// Pause for confirmation.
	system( "PAUSE" );

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

	try {
		cout << "Parsing item definitions... ";
		itemDefinitions = KeyValueParser::Parse( itemDefinition );
		cout << "done!" << endl;

		cout << "Parsing language definitions... ";
		langDefinitions = KeyValueParser::Parse( langDefinition );
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
		string *itemName, *realName, *itemIndex, *slotName, *texturePath, *textureUrl;
		try
		{
			itemName = thisTable->getString("item_name");
			itemIndex = thisTable->getString("defindex");
			slotName = thisTable->getString("item_slot");
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
		thisObject["image"] = *texturePath;

		// Create slot.
		EItemSlot itemSlot;
		if (*slotName == "invalid") {
			itemSlot = SLOT_INVALID;
		}
		else if (*slotName == "primary") {
			itemSlot = SLOT_PRIMARY;
		}
		else if (*slotName == "secondary") {
			itemSlot = SLOT_SECONDARY;
		}
		else if (*slotName == "melee") {
			itemSlot = SLOT_MELEE;
		}
		else if (*slotName == "pda") {
			itemSlot = SLOT_PDA;
		}
		else if (*slotName == "pda2") {
			itemSlot = SLOT_PDA2;
		}
		else if (*slotName == "building") {
			itemSlot = SLOT_BUILDING;
		}
		else if (*slotName == "head") {
			itemSlot = SLOT_HEAD;
		}
		else if (*slotName == "misc") {
			itemSlot = SLOT_MISC;
		}
		else if (*slotName == "action") {
			itemSlot = SLOT_ACTION;
		}
		else if (*slotName == "grenade") {
			itemSlot = SLOT_GRENADE;
		}
		else {
			throw Exception( "Failed to parse item definition. Unexpected item slot type found." );
		}
		thisObject["slot"] = itemSlot;

		unsigned int allClasses = CLASS_ALL;
		Hashtable *usedByTable = NULL, *classTable = NULL;
		try
		{
			usedByTable = thisTable->getTable("used_by_classes");
		} catch (Exception tableException)
		{
			// Item is used by all classes.
			thisObject["classes"] = allClasses;
		}

		if (usedByTable != nullptr) {
			try {
				classTable = usedByTable->getTable("class");
			}
			catch (Exception classException) {
				throw Exception("Unexpected definition format. " + *realName + " has class usage, but no classes defined.");
			}

			stringMap::iterator i;
			unsigned int classFlags = CLASS_NONE;
			for (i = classTable->begin(); i != classTable->end(); i++) {
				string *className = nullptr;
				try {
					className = boost::any_cast<string*>(i->second);
				}
				catch (boost::bad_any_cast&) {
					throw Exception( "Failed to parse class usage." );
				}

				// Add all classes.
				if (*className == "Scout")
					classFlags |= CLASS_SCOUT;
				if (*className == "Soldier")
					classFlags |= CLASS_SOLDIER;
				if (*className == "Pyro")
					classFlags |= CLASS_PYRO;
				if (*className == "Demoman")
					classFlags |= CLASS_DEMOMAN;
				if (*className == "Heavy")
					classFlags |= CLASS_HEAVY;
				if (*className == "Engineer")
					classFlags |= CLASS_ENGINEER;
				if (*className == "Medic")
					classFlags |= CLASS_MEDIC;
				if (*className == "Sniper")
					classFlags |= CLASS_SNIPER;
				if (*className == "Spy")
					classFlags |= CLASS_SPY;
			}

			thisObject["classes"] = classFlags;
		}

		root.append( thisObject );
	}

	// Add definition for unknown item.
	Json::Value unknownItem;
	unknownItem["index"] = "-1";
	unknownItem["name"] = "Unknown Item";
	unknownItem["slot"] = SLOT_INVALID;
	unknownItem["image"] = "backpack/unknown_item";
	unknownItem["classes"] = 0;

	// Set -1 to be unknown index.
	root.append( unknownItem );

	ofstream jsonOutput( "itemDefinitions.json" );
	if (!jsonOutput) {
		throw Exception( "Couldn't write to file." );
	}

	string jsonText = jsonWriter.write( root );
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