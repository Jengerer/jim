#include "equip_status.hpp"

/*
 * Constructor
 */
EquippedStatus::EquippedStatus( EEquipClass equip_class,
		EEquipSlot equip_slot )
{
	// Set basic attributes.
	set_equip_class( equip_class );
	set_equip_slot( equip_slot );
}

/*
 * Destructor
 */
EquippedStatus::~EquippedStatus( void )
{
	
}

EEquipClass EquippedStatus::get_equip_class( void ) const
{
	return equip_class_;
}

EEquipSlot EquippedStatus::get_equip_slot( void ) const
{
	return equip_slot_;
}

void EquippedStatus::set_equip_class( EEquipClass equip_class )
{
	equip_class_ = equip_class;
}

void EquippedStatus::set_equip_slot( EEquipSlot equip_slot )
{
	equip_slot_ = equip_slot;
}
