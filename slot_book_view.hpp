#ifndef SLOT_GRID_PAGES_HPP
#define SLOT_GRID_PAGES_HPP

#include <jui/layout/constrained_container.hpp>
#include <jui/layout/horizontal_layout.hpp>
#include <jui/io/mouse.hpp>

#include "slot_array.hpp"
#include "slot_book.hpp"
#include "slot_grid_view.hpp"

class SlotBookView : public JUI::ConstrainedContainer
{

public:

	SlotBookView( const SlotBook* slot_book,
		unsigned int slot_spacing );
	virtual ~SlotBookView( void );

	// Layout functions.
	void pack( void );

	// Page management.
	bool initialize( void );
	bool update_pages( void );
	unsigned int get_active_page( void ) const;
	unsigned int get_page_count( void ) const;
	SlotGridView* get_active_view( void ) const;

	// JUI::Mouse* handling.
	SlotView* get_touching_slot( JUI::Mouse* mouse ) const;
	SlotView* get_slot_view( unsigned int index ) const;

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

	// Navigation functions.
	void set_active_page( unsigned int page );
	void set_view_offset( float view_offset );

protected:

	// Navigation members.
	JUI::HorizontalLayout* pages_layout_;
	JUI::Constraint* pages_constraint_;
	float view_offset_;

private:

	// Data member.
	const SlotBook* slot_book_;

	// Layout attributes.
	unsigned int slot_spacing_;

	// View offset.
	unsigned int page_;
	JUTIL::Vector<SlotGridView*> slot_grid_views_;

};

#endif // SLOT_GRID_PAGES_HPP