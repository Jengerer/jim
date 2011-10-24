#ifndef STEAM_ITEM_HANDLER_H
#define STEAM_ITEM_HANDLER_H

#include "slot_view.h"
#include "steam.h"

#include <set>

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
	void select( SlotView* slotView );
	void deselect( SlotView* slotView );
	void toggle_select( SlotView* slotView );
	void deselect_all();
	bool is_selected( SlotView* select ) const;
	size_t get_selected_count() const;
	void set_select_mode( SelectMode selectMode );
	SelectMode get_select_mode() const;

	// Item handling.
	void update_item( const Item* item ) const;

	// Handling Steam updating.
	bool is_selected_tradable( void ) const;
	void craft_selected( void );
	void equip_selected( EClassEquip whichClass, bool setEquip );
	bool can_equip_selected() const;

private:

	std::set<SlotView*> selected_;
	SelectMode selectMode_;

};

#endif // STEAM_ITEM_HANDLER_H