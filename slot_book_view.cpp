#include "slot_book_view.h"

const unsigned int DEFAULT_PAGE = 0;
const float DEFAULT_VIEW_OFFSET = 0.0f;

/*
 * Slot book view constructor.
 */
SlotBookView::SlotBookView( const SlotBook* slot_book,
	unsigned int page_spacing,
	unsigned int slot_spacing )
{
	set_slot_book( slot_book );
	set_slot_spacing( slot_spacing );

	set_view_offset( DEFAULT_VIEW_OFFSET );
	set_active_page( DEFAULT_PAGE );

	// Create layout.
	pages_layout_ = new HorizontalLayout( page_spacing, ALIGN_TOP );
	pages_layout_->set_parent( this );
	add( pages_layout_ );

	// Constrain and pack.
	update_pages();
	pages_constraint_ = set_constraint( pages_layout_, get_view_offset(), 0.0f );
	pack();
}

/*
 * Pack layout.
 */
void SlotBookView::pack( void )
{
	pages_layout_->pack();
	
	// Set size as active page (should all be equal).
	bool has_page = !slot_grid_views_.empty();
	int width = (has_page ? get_active_view()->get_width() : 0 );
	int height = (has_page ? get_active_view()->get_height() : 0);
	set_size( width, height );
}

/*
 * Update pages in book view.
 */
void SlotBookView::update_pages( void )
{
	// Add missing pages.
	const SlotBook* slot_book = get_slot_book();
	unsigned int current_pages = get_page_count();
	unsigned int needed_pages = slot_book->get_page_count();
	for (unsigned int i = current_pages; i < needed_pages; i++) {
		const SlotArray* page = slot_book->get_page( i );
		
		// Create grid.
		SlotGridView* grid_view = new SlotGridView( page,
			slot_book->get_page_width(),
			get_slot_spacing() );
		pages_layout_->add( grid_view );
		slot_grid_views_.push_back( grid_view );
	}

	pack();
}

/*
 * Get the active page index.
 */
unsigned int SlotBookView::get_active_page( void ) const
{
	return page_;
}

/*
 * Get number of page views.
 */
unsigned int SlotBookView::get_page_count( void ) const
{
	return slot_grid_views_.size();
}

/*
 * Get the active page view.
 */
SlotGridView* SlotBookView::get_active_view( void ) const
{
	return slot_grid_views_[ get_active_page() ];
}

/*
 * Get the slot view being touched by mouse.
 */
SlotView* SlotBookView::get_touching_slot( Mouse* mouse ) const
{
	// Only check current page.
	if (get_page_count() != 0) {
		SlotGridView* current_page = get_active_view();
		return current_page->get_touching_slot( mouse );
	}

	return nullptr;
}

/*
 * Move to next page. Return true if we moved to
 * next page.
 */
bool SlotBookView::next_page( void )
{
	unsigned int current_page = get_active_page();
	current_page++;

	// Only update if within bound.
	if (current_page < get_page_count()) {
		set_active_page( current_page );
		return true;
	}

	return false;
}

/*
 * Move to previous page. Return true if we moved to
 * previous page.
 */
bool SlotBookView::previous_page( void )
{
	unsigned int current_page = get_active_page();

	// Only update if not already at left-most page.
	if (current_page > 0) {
		current_page--;
		set_active_page( current_page );
		return true;
	}

	return false;
}

/*
 * Update offset for book view.
 */
void SlotBookView::update_offset()
{
	// Move offset to acitve page.
	if (get_page_count() != 0) {
		float new_offset = pages_layout_->get_x() - get_active_view()->get_x();
		set_view_offset( new_offset );
	}
}

/*
 * Update book view.
 */
void SlotBookView::update_view()
{
	// Set constraint.
	pages_constraint_->set_constraint( get_view_offset(), 0.0f );
	apply_constraint( pages_constraint_ );
}

/*
 * Enable all slot views.
 */
void SlotBookView::set_enabled( bool is_enabled ) const
{
	for each (SlotGridView* grid_view in slot_grid_views_) {
		grid_view->set_enabled( is_enabled );
	}
}

/*
 * Disable all slot views with an item.
 */
void SlotBookView::disable_full( void ) const
{
	for each (SlotGridView* grid_view in slot_grid_views_) {
		grid_view->disable_full();
	}
}

/*
 * Set book to be viewed.
 */
void SlotBookView::set_slot_book( const SlotBook* slot_book )
{
	slot_book_ = slot_book;
}

/*
 * Get slot book being managed.
 */
const SlotBook* SlotBookView::get_slot_book( void ) const
{
	return slot_book_;
}

/*
 * Set spacing between slots.
 */
void SlotBookView::set_slot_spacing( unsigned int slot_spacing )
{
	slot_spacing_ = slot_spacing;
}

/*
 * Get spacing between slots.
 */
unsigned int SlotBookView::get_slot_spacing( void ) const
{
	return slot_spacing_;
}

/*
 * Set active page index viewed.
 */
void SlotBookView::set_active_page( unsigned int page )
{
	page_ = page;
	update_offset();
}

/*
 * Set offset for book pages view.
 */
void SlotBookView::set_view_offset( float view_offset )
{
	view_offset_ = view_offset;
}

/*
 * Get offset for book pages view.
 */
float SlotBookView::get_view_offset( void ) const
{
	return view_offset_;
}