#ifndef EQUIPPED_STATUS_H
#define EQUIPPED_STATUS_H

#include "steam/SteamAPI.h"
#include "steam/SteamTypes.h"
#include "steam/UserItemsCommon.h"
#include "item_shared.hpp"
#include "item_definition.hpp"

#include <containers/vector.hpp>
#include <containers/map.hpp>
#include <string/string.hpp>
#include <string/constant_string.hpp>
#include <jui/gfx/texture.hpp>

class EquippedStatus
{
public:
	// Constructor.
	EquippedStatus( EEquipClass equip_class,
		EEquipSlot equip_slot );
	~EquippedStatus( void );

	// Equip setters and getters.
	void set_equip_slot( EEquipSlot equip_slot );
	EEquipClass get_equip_class( void ) const;
	EEquipSlot get_equip_slot( void ) const;

private:

	// Should add a new attribute for a new class, don't override.
	void set_equip_class( EEquipClass equip_class );

private:

	EEquipClass equip_class_;
	EEquipSlot equip_slot_;

};

#endif // EQUIPPED_STATUS_H