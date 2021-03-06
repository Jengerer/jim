#ifndef ITEM_DEFINITIONS_H
#define ITEM_DEFINITIONS_H

#include <jui/gfx/colour.hpp>
#include "steam/SteamTypes.h"

enum EInventoryClass {
	INVENTORY_CLASS_NONE		= 0x00000000,
	INVENTORY_CLASS_SCOUT		= 0x00010000,
	INVENTORY_CLASS_SOLDIER		= 0x00040000,
	INVENTORY_CLASS_PYRO		= 0x00400000,
	INVENTORY_CLASS_DEMOMAN		= 0x00080000,
	INVENTORY_CLASS_HEAVY		= 0x00200000,
	INVENTORY_CLASS_ENGINEER	= 0x01000000,
	INVENTORY_CLASS_MEDIC		= 0x00100000,
	INVENTORY_CLASS_SNIPER		= 0x00020000,
	INVENTORY_CLASS_SPY			= 0x00800000,
	INVENTORY_CLASS_ALL			= 0x01FF0000
};

enum EItemSlot {
	ITEM_SLOT_NONE = 0,
	ITEM_SLOT_INVALID,
	ITEM_SLOT_PRIMARY,
	ITEM_SLOT_SECONDARY,
	ITEM_SLOT_MELEE,
	ITEM_SLOT_PDA,
	ITEM_SLOT_PDA2,
	ITEM_SLOT_BUILDING,
	ITEM_SLOT_HEAD,
	ITEM_SLOT_MISC,
	ITEM_SLOT_ACTION,
	ITEM_SLOT_GRENADE
};

enum EEquipClass {
	EQUIP_CLASS_SCOUT_UNUSED = 0,
	EQUIP_CLASS_SCOUT,
	EQUIP_CLASS_SNIPER,
	EQUIP_CLASS_SOLDIER,
	EQUIP_CLASS_DEMOMAN,
	EQUIP_CLASS_MEDIC,
	EQUIP_CLASS_HEAVY,
	EQUIP_CLASS_PYRO,
	EQUIP_CLASS_SPY,
	EQUIP_CLASS_ENGINEER,
};

enum EEquipSlot {
	EQUIP_SLOT_PRIMARY = 0,
	EQUIP_SLOT_SECONDARY,
	EQUIP_SLOT_MELEE,
	EQUIP_SLOT_UNUSED_1,
	EQUIP_SLOT_BUILDING,
	EQUIP_SLOT_UNUSED_2,
	EQUIP_SLOT_PDA2,
	EQUIP_SLOT_HEAD,
	EQUIP_SLOT_MISC,
	EQUIP_SLOT_ACTION,
	EQUIP_SLOT_MISC_2,

	EQUIP_SLOT_PREVIOUS = 65535
};

enum EItemOrigins {
	ORIGIN_ACHIEVEMENT = 1,
	ORIGIN_CRAFTED = 4
};

enum EItemToolType {
	TOOL_INVALID,
	TOOL_GIFT,
	TOOL_DUEL_MINIGAME,
	TOOL_NOISE_MAKER,
	TOOL_NAME_TAG,
	TOOL_KEY,
	TOOL_CRATE,
	TOOL_PAINT_CAN,
	TOOL_DECAL,
	TOOL_GIFT_WRAP,
	TOOL_WRAPPED_GIFT,
	TOOL_DESCRIPTION_TAG,
	TOOL_BACKPACK_EXPANDER,
	TOOL_WEDDING_RING,
	TOOL_ACCOUNT_UPGRADE,
	TOOL_CLAIMCODE,
	TOOL_STRANGE_PART,
	TOOL_STRANGE_PART_RESTRICTION,
	TOOL_SPELL
};

const JUI::Colour QUALITY_NORMAL_COLOUR		= JUI::Colour( 0xB2B2B2 );
const JUI::Colour QUALITY_COMMON_COLOUR		= JUI::Colour( 0xFFD700 );
const JUI::Colour QUALITY_VINTAGE_COLOUR	= JUI::Colour( 0x476291 );
const JUI::Colour QUALITY_GENUINE_COLOUR	= JUI::Colour( 0x4D7455 );
const JUI::Colour QUALITY_STRANGE_COLOUR	= JUI::Colour( 0xCF6A32 );
const JUI::Colour QUALITY_UNUSUAL_COLOUR	= JUI::Colour( 0x8650AC );
const JUI::Colour QUALITY_HAUNTED_COLOUR	= JUI::Colour( 0x38F3AB );
const JUI::Colour QUALITY_COMMUNITY_COLOUR	= JUI::Colour( 0x70B04A );
const JUI::Colour QUALITY_VALVE_COLOUR		= JUI::Colour( 0xA50F79 );
const JUI::Colour QUALITY_COLLECTORS_COLOUR	= JUI::Colour( 0xAA0000 );

// Inventory flags.
#define FL_ITEM_ALL			0xFFFFFFFF
#define FL_ITEM_VALID		0x80000000
#define FL_ITEM_NEW			0x00000000
#define FL_ITEM_POSITION	0x0000FFFF
#define FL_ITEM_NONPOSITION	(FL_ITEM_ALL ^ FL_ITEM_POSITION)

// Property flags
#define FL_ITEM_NOT_CRATE	0xFFFFFFFF
#define FL_ITEM_NOT_PAINTED	0xFFFFFFFF
#define FL_ITEM_NOT_STRANGE 0xFFFFFFFF
#define FL_ITEM_UNLIMITED_COUNT 0xFFFFFFFF

#endif // ITEM_DEFINITIONS_H