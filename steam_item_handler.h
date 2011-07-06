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

	// Selection handling.
	void Select( SlotView* slotView );
	void Deselect( SlotView* slotView );
	void DeselectAll();
	bool IsSelected( SlotView* select ) const;
	size_t GetSelectedCount() const;
	void SetSelectMode( SelectMode selectMode );
	SelectMode GetSelectMode() const;

	// Item handling.
	void UpdateItem( const Item* item ) const;

	// Handling Steam updating.
	void CraftSelected();
	void EquipSelected( EClassEquip whichClass, bool setEquip );

private:

	std::set<SlotView*> selected_;
	SelectMode selectMode_;

};

#endif // STEAM_ITEM_HANDLER_H