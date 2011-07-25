#include "parser.h"

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

}

void exitApplication()
{
	cout << "Deallocating memory stored in tables... ";
	cout << "done!" << endl;
}