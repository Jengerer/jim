#ifndef SLOT_GRID_PAGES_H
#define SLOT_GRID_PAGES_H

#include <jui/constrained_container.h>
#include <jui/horizontal_layout.h>
#include <jui/mouse.h>

#include "slot_array.h"
#include "slot_book.h"
#include "slot_grid_view.h"

class SlotBookView : public ConstrainedContainer
{

public:

	SlotBookView( const SlotBook* slot_book,
		unsigned int page_spacing,
		unsigned int slot_spacing );

	// Layout functions.
	void pack( void );

	// Page management.
	void update_pages( void );
	unsigned int get_active_page( void ) const;
	unsigned int get_page_count( void ) const;
	SlotGridView* get_active_view( void ) const;

	// Mouse handling.
	SlotView* get_touching_slot( Mouse* mouse ) const;

	// Page navigation.
	bool next_page( void );
	bool previous_page( void );
	void update_offset( void );
	virtual void update_view( void );

	// Slot view handling.
	void set_enabled( bool is_enabled ) const;
	void disable_full( void ) const;

protected:

	// Navigation functions.
	float get_view_offset( void ) const;

private:

	// Book management.
	void set_slot_book( const SlotBook* slot_book );
	const SlotBook* get_slot_book( void ) const;

	// Layout functions.
	void set_slot_spacing( unsigned int slot_spacing );
	unsigned int get_slot_spacing( void ) const;

	// Navigation functions.
	void set_active_page( unsigned int page );
	void set_view_offset( float view_offset );

protected:

	// Navigation members.
	HorizontalLayout*		pages_layout_;
	Constraint*				pages_constraint_;
	float					view_offset_;

private:

	// Data member.
	const SlotBook* slot_book_;

	// Layout attributes.
	unsigned int slot_spacing_;

	// View offset.
	unsigned int page_;
	std::vector<SlotGridView*> slot_grid_views_;

};

#endif // SLOT_GRID_PAGES_H