#include "backpack.h"

#include <json/json.h>

#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"
#include "protobuf/steammessages.pb.h"

Backpack::Backpack( SlotBook* book, DynamicSlotBook* excluded_book ) : Inventory( book, excluded_book )
{
	set_loaded( false );
}

bool Backpack::is_loaded( void ) const
{
	return is_loaded_;
}

void Backpack::set_loaded( bool isLoaded )
{
	is_loaded_ = isLoaded;
}