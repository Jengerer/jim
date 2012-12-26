#ifndef STEAM_ITEM_HANDLER_H
#define STEAM_ITEM_HANDLER_H

#include "slot_view.hpp"
#include "steam.hpp"
#include <containers/set.hpp>

enum SelectMode
{
	SELECT_MODE_SINGLE,
	SELECT_MODE_MULTIPLE
};

class SteamItemHandler : public Steam
{
public:

	SteamItemHandler();
	virtual ~SteamItemHandler();

	// Selection handling.
	bool select( SlotView* slot_view );
	void deselect( SlotView* slot_view );
	bool toggle_select( SlotView* slot_view );
	void deselect_all( void );
	bool is_selected( SlotView* select ) const;
	size_t get_selected_count( void ) const;
	void set_select_mode( SelectMode selectMode );
	SelectMode get_select_mode( void ) const;

	// Item handling.
	void update_item( const Item* item ) const;

	// Handling Steam updating.
	bool is_selected_tradable( void ) const;
	bool craft_selected( void );
	void equip_selected( EInventoryClass whichClass, bool setEquip );
	bool can_equip_selected( void ) const;

private:

	JUTIL::Set<SlotView*> selected_;
	SelectMode select_mode_;

};

#endif // STEAM_ITEM_HANDLER_H