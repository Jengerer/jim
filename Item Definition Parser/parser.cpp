#include "Parser.h"

#include <string>
#include <iostream>
#include <fstream>
#include <json/json.h>

void loadDefinitions();
void exitApplication();

Json::Value GetMember( Json::Value& value, const string& member )
{
	Json::Value defaultValue;
	if (!value.isMember( member )) {
		// TODO: member_not_found exception.
		throw Exception( "Unexpected definition format. Element '" + member + "' not found." );
	}

	return value.get( member, defaultValue );
}

int main()
{
	try {
		loadDefinitions();
	}
	catch (Exception defineException) {
		cout << "FAILED!\n";
		cout << "Exception: " << *defineException.getMessage() << endl;
	}

	exitApplication();

	// Pause for confirmation.
	system( "PAUSE" );

	return 0;
}

void loadDefinitions()
{
	cout << "Starting item definition parser.\n";

	cout << "Opening connection interface... ";
	Curl* curl = new Curl();
	cout << "OK!\n";

	cout << "Reading item definitions... ";
	string itemDefinition = curl->read( "http://api.steampowered.com/IEconItems_440/GetSchema/v0001/?key=0270F315C25E569307FEBDB67A497A2E&format=json&language=en" );
	cout << "OK!\n";

	cout << "Parsing item definitions... ";

	Json::Reader reader;
	Json::Value definitionRoot;
	if (!reader.parse( itemDefinition, definitionRoot, false )) {
		throw Exception( "Failed to parse item definition file." );
	}

	cout << "OK!\n";
	cout << "Generating definitions... ";

	Json::Value result = GetMember( definitionRoot, "result" );
	Json::Value items = GetMember( result, "items" );

	// Create JSON output variables.
	Json::Value itemsOutput;

	for (Json::ValueIterator i = items.begin(); i != items.end(); ++i) {
		Json::Value item = *i;

		// Check that all necessary attributes exist.
		unsigned int defindex	= GetMember( item, "defindex" ).asUInt();
		string item_name		= GetMember( item, "item_name" ).asString();
		string item_slot		= GetMember( item, "item_slot" ).asString();
		string image_inventory	= GetMember( item, "image_inventory" ).asString();
		string image_url		= GetMember( item, "image_url" ).asString();

		if (image_inventory.empty() || image_url.empty()) {
			image_inventory = "backpack/unknown_item";
			image_url = "http://www.jengerer.com/itemManager/imgFiles/backpack/unknown_item.png";
		}

		if (!curl->download( image_url, "imgFiles/" + image_inventory + ".png" )) {
			throw Exception( "Failed to download file '" + image_url + "'." );
		}

		Json::Value thisItem;
		thisItem["index"] = defindex;
		thisItem["image"] = image_inventory;
		thisItem["name"] = item_name;

		// Create slot.
		EItemSlot itemSlot;
		if (item_slot.empty()) {
			itemSlot = SLOT_NONE;
		}
		else if (item_slot == "invalid") {
			itemSlot = SLOT_INVALID;
		}
		else if (item_slot == "primary") {
			itemSlot = SLOT_PRIMARY;
		}
		else if (item_slot == "secondary") {
			itemSlot = SLOT_SECONDARY;
		}
		else if (item_slot == "melee") {
			itemSlot = SLOT_MELEE;
		}
		else if (item_slot == "pda") {
			itemSlot = SLOT_PDA;
		}
		else if (item_slot == "pda2") {
			itemSlot = SLOT_PDA2;
		}
		else if (item_slot == "building") {
			itemSlot = SLOT_BUILDING;
		}
		else if (item_slot == "head") {
			itemSlot = SLOT_HEAD;
		}
		else if (item_slot == "misc") {
			itemSlot = SLOT_MISC;
		}
		else if (item_slot == "action") {
			itemSlot = SLOT_ACTION;
		}
		else if (item_slot == "grenade") {
			itemSlot = SLOT_GRENADE;
		}
		else {
			throw Exception( "Failed to parse item definition. Unexpected item slot type found." );
		}

		thisItem["slot"] = itemSlot;

		// Get classes, if they exist.
		if (!item.isMember( "used_by_classes" )) {
			thisItem["classes"] = CLASS_ALL;
		}
		else {
			unsigned int classFlags = CLASS_NONE;
			Json::Value classes = GetMember( item, "used_by_classes" );
			for (Json::ValueIterator i = classes.begin(); i != classes.end(); i++) {
				string className = (*i).asString();

				// Add all classes.
				if (!className.empty()) {
					if (className == "Scout")
						classFlags |= CLASS_SCOUT;
					else if (className == "Soldier")
						classFlags |= CLASS_SOLDIER;
					else if (className == "Pyro")
						classFlags |= CLASS_PYRO;
					else if (className == "Demoman")
						classFlags |= CLASS_DEMOMAN;
					else if (className == "Heavy")
						classFlags |= CLASS_HEAVY;
					else if (className == "Engineer")
						classFlags |= CLASS_ENGINEER;
					else if (className == "Medic")
						classFlags |= CLASS_MEDIC;
					else if (className == "Sniper")
						classFlags |= CLASS_SNIPER;
					else if (className == "Spy")
						classFlags |= CLASS_SPY;
					else
						throw Exception( "Unexpected class name '" + className + "' found." );
				}
			}

			thisItem["classes"] = classFlags;
		}

		itemsOutput.append( thisItem );
	}

	cout << "OK!\n";


	cout << "Generating output file... ";

	// Create root object, add items object.
	Json::Value outputRoot;
	outputRoot["items"] = itemsOutput;

	// Print root to file.
	Json::StyledWriter writer;
	ofstream outputStream;
	outputStream.open( "item_definitions.json" );
	if (outputStream.bad()) {
		throw Exception( "Failed to open output file to write." );
	}
	outputStream << writer.write( outputRoot );
	outputStream.close();

	cout << "OK!\n";
	cout << "Item definitions generated successfully!\n";
}

void exitApplication()
{
	cout << "Deallocating memory stored in tables... ";
	cout << "done!" << endl;
}