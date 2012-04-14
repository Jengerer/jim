#include "dynamic_slot_book.h"

/*
 * Dynamic slot book constructor.
 */
DynamicSlotBook::DynamicSlotBook( unsigned int width,
	unsigned int height,
	unsigned int min_pages ) : SlotBook( width, height )
{
	set_min_pages( min_pages );
	add_pages( min_pages );
}

/*
 * Add an item to the book.
 */
void DynamicSlotBook::insert_item( Item* item )
{
	// Find empty slot.
	unsigned int end = get_slot_count();
	for (unsigned int i = 0; i < end; i++) {
		if (is_slot_empty( i )) {
			SlotBook::insert_item( item, i );
			return;
		}
	}

	// Add to new page if couldn't fit.
	const SlotArray* new_page = add_page();
	new_page->insert_item( item, 0 );
}

/*
 * Remove an item from the book, pruning empty pages.
 */
void DynamicSlotBook::remove_item( Item* item )
{
	// First remove item from slot.
	unsigned int previous_index = item->get_index();
	Slot* slot = get_slot( previous_index );
	slot->remove_item();

	// Now move all items after left.
	unsigned int end = get_slot_count();
	for (unsigned int i = previous_index + 1; i < end; ++i) {
		Slot* current_slot = get_slot( i );
		if (current_slot->has_item()) {
			Item* current_item = current_slot->get_item();
			current_slot->remove_item();

			// Move item to the previous slot.
			SlotBook::insert_item( current_item, previous_index );
			previous_index = i;
		}
		else {
			// Assuming it was pruned before, the removed item
			// should be the only gap.
			break;
		}
	}

	// Remove last page if empty and not minimal.
	if (get_page_count() > get_min_pages()) {
		unsigned int last_index = get_page_count() - 1;
		const SlotArray* last_page = get_page( last_index );
		if (last_page->is_empty()) {
			remove_page( last_index );
		}
	}
}

/*
 * Get minimum number of pages.
 */
unsigned int DynamicSlotBook::get_min_pages( void ) const
{
	return min_pages_;
}

/*
 * Set minimum number of pages.
 */
void DynamicSlotBook::set_min_pages( unsigned int min_pages )
{
	min_pages = min_pages;
}