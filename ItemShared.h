#ifndef ITEM_DEFINITIONS_H
#define ITEM_DEFINITIONS_H

#include "Texture.h"

// TODO: Maybe an enum doesn't make sense.
enum EClassEquip {
	CLASS_NONE		= 0x00000000,
	CLASS_SCOUT		= 0x00010000,
	CLASS_SOLDIER	= 0x00040000,
	CLASS_PYRO		= 0x00400000,
	CLASS_DEMOMAN	= 0x00080000,
	CLASS_HEAVY		= 0x00200000,
	CLASS_ENGINEER	= 0x01000000,
	CLASS_MEDIC		= 0x00100000,
	CLASS_SNIPER	= 0x00020000,
	CLASS_SPY		= 0x00800000,
	CLASS_ALL		= 0x01FF0000
};

enum EItemSlot {
	SLOT_INVALID = 0,
	SLOT_PRIMARY,
	SLOT_SECONDARY,
	SLOT_MELEE,
	SLOT_PDA,
	SLOT_PDA2,
	SLOT_BUILDING,
	SLOT_HEAD,
	SLOT_MISC,
	SLOT_ACTION,
	SLOT_GRENADE
};

// Inventory flags.
#define FL_ITEM_ALL			0xFFFFFFFF
#define FL_ITEM_VALID		0x80000000
#define FL_ITEM_NEW			0x00000000
#define FL_ITEM_POSITION	0x0000FFFF
#define FL_ITEM_NONPOSITION	(FL_ITEM_ALL ^ FL_ITEM_POSITION)

#endif // ITEM_DEFINITIONS_H