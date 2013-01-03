#include "slot_book_view.hpp"

const unsigned int DEFAULT_PAGE = 0;
const float DEFAULT_VIEW_OFFSET = 0.0f;

/*
 * Slot book view constructor.
 */
SlotBookView::SlotBookView( const SlotBook* slot_book,
	unsigned int slot_spacing )
{
	set_slot_book( slot_book );
	set_view_offset( DEFAULT_VIEW_OFFSET );
	set_active_page( DEFAULT_PAGE );
	set_shift_multiple( SLOT_BOOK_VIEW_SHIFT_OFF );

    // Layout parameters.
    slot_spacing_ = slot_spacing;

	// Pages to be created.
	pages_layout_ = nullptr;
	pages_constraint_ = nullptr;

}

/*
 * Slot book view constructor.
 */
SlotBookView::~SlotBookView( void )
{
}

/*
 * Pack layout.
 */
void SlotBookView::pack( void )
{
	pages_layout_->pack();
	
	// Set size as active page (should all be equal).
	bool has_page = !slot_grid_views_.is_empty();
	int width = (has_page ? get_active_view()->get_width() : 0 );
	int height = (has_page ? get_active_view()->get_height() : 0);
	set_size( width, height );
}

/*
 * Create UI elements.
 */
bool SlotBookView::initialize( void )
{
	// Create layout.
	if (!JUTIL::BaseAllocator::allocate( &pages_layout_ )) {
		return false;
	}
	pages_layout_ = new (pages_layout_) JUI::HorizontalLayout( 0, JUI::ALIGN_TOP );
	pages_layout_->set_parent( this );
	if (!add( pages_layout_ )) {
        JUTIL::BaseAllocator::destroy( pages_layout_ );
        return false;
    }

	// Constrain and pack.
	if (!update_pages()) {
        return false;
    }
    int view_offset = static_cast<int>(get_view_offset());
	pages_constraint_ = set_constraint( pages_layout_, view_offset, 0 );
	pack();
    return true;
}

/*
 * Update pages in book view.
 * If fails to add pages, returns false.
 * NOTE: If adding pages was partially successful, those pages
 * remain in book view.
 */
bool SlotBookView::update_pages( void )
{
	// Add missing pages.
	const SlotBook* slot_book = get_slot_book();
	unsigned int current_pages = get_page_count();
	unsigned int needed_pages = slot_book->get_page_count();
	for (unsigned int i = current_pages; i < needed_pages; i++) {
		const SlotArray* page = slot_book->get_page( i );
		
		// Create grid.
        SlotGridView* grid_view;
        if (!JUTIL::BaseAllocator::allocate( &grid_view )) {
            return false;
        }
		grid_view = new (grid_view) SlotGridView( slot_book->get_page_width(), slot_spacing_ );

		// Add to layout.
		if (!pages_layout_->add( grid_view )) {
            JUTIL::BaseAllocator::destroy( grid_view );
            return false;
        }

		// Add page to grid.
		if (!grid_view->add_slots( page )) {
			return false;
		}

		// Add to vector.
		if (!slot_grid_views_.push( grid_view )) {
            return false;
        }
	}

    // Organize in layout.
	pack();
    return true;
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
	return slot_grid_views_.get_length();
}

/*
 * Get the active page view.
 */
SlotGridView* SlotBookView::get_active_view( void ) const
{
    unsigned int active_page = get_active_page();
	return slot_grid_views_.get( active_page );
}

/*
 * Get the slot view being touched by mouse.
 */
SlotView* SlotBookView::get_touching_slot( JUI::Mouse* mouse ) const
{
	// Only check current page.
	if (get_page_count() != 0) {
		SlotGridView* current_page = get_active_view();
		return current_page->get_touching_slot( mouse );
	}

	return nullptr;
}

/*
 * Get slot view by index.
 */
SlotView* SlotBookView::get_slot_view( unsigned int index ) const
{
	// Get page.
	unsigned int page_size = slot_book_->get_page_size();
	unsigned int page = index / page_size;
	unsigned int page_index = index % page_size;
	SlotGridView* grid_page = slot_grid_views_.get( page );
	return grid_page->get_slot_view( page_index );
}

/*
 * Move to next page. Return true if we moved to
 * next page.
 */
bool SlotBookView::next_page( void )
{
	unsigned int current_page = get_active_page();
	current_page += get_shift_multiple();

	// Only update if within bound.
	if (current_page < get_page_count()) {
		set_active_page( current_page );
		return true;
	}else{
		return last_page();
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

	// Only update if within bound.
	if (current_page >= get_shift_multiple()) {
		current_page -= get_shift_multiple();
		set_active_page( current_page );
		return true;
	}else{
		return first_page();
	}

	return false;
}

/*
 * Move to first page. Return true if we moved to
 * first page.
 */
bool SlotBookView::first_page( void )
{
	set_active_page( 0 );
	return true;
}

/*
 * Move to last page. Return true if we moved to
 * last page.
 */
bool SlotBookView::last_page( void )
{
	set_active_page( get_page_count() - 1 );
	return true;
}

bool SlotBookView::jump_to_page( unsigned int digit )
{
	if( digit >= SLOT_BOOK_VIEW_SHIFT_DIFF ){
		return false;
	}
	unsigned int current_page = get_active_page() + 1; // One based origin for user interaction
	unsigned int lower_part = current_page % shift_multiple_;
	unsigned int upper_part = current_page / (shift_multiple_ * SLOT_BOOK_VIEW_SHIFT_DIFF);
	// Strip the current level
	current_page = lower_part + (upper_part * (shift_multiple_ * SLOT_BOOK_VIEW_SHIFT_DIFF));
	current_page += digit * shift_multiple_;
	if( current_page < 1 ){
		return first_page();
	}
	if( current_page > get_page_count() ){
		return last_page();
	}
	set_active_page( current_page - 1);
	return true;
}

unsigned int SlotBookView::get_shift_multiple( void ) const
{
	return shift_multiple_;
}

void SlotBookView::set_shift_multiple( unsigned int multiple )
{
	shift_multiple_ = multiple;
}

/*
 * Update offset for book view.
 */
void SlotBookView::update_offset( void )
{
	// Move offset to acitve page.
	if (get_page_count() != 0) {
		int new_offset = pages_layout_->get_x() - get_active_view()->get_x();
		set_view_offset( static_cast<float>(new_offset) );
	}
}

/*
 * Update book view.
 */
void SlotBookView::update_view( void )
{
	// Set constraint.
    int view_offset = static_cast<int>(get_view_offset());
	pages_constraint_->set_constraint( view_offset, 0 );
	apply_constraint( pages_constraint_ );
}

/*
 * Enable all slot views.
 */
void SlotBookView::set_enabled( bool is_enabled ) const
{
    // Enable all slot views.
    size_t i;
    size_t length = slot_grid_views_.get_length();
    for (i = 0; i < length; ++i) {
        SlotGridView* grid_view = slot_grid_views_.get( i );
		grid_view->set_enabled( is_enabled );
	}
}

/*
 * Disable all slot views with an item.
 */
void SlotBookView::disable_full( void ) const
{
	// Enable all slot views.
    size_t i;
    size_t length = slot_grid_views_.get_length();
    for (i = 0; i < length; ++i) {
        SlotGridView* grid_view = slot_grid_views_.get( i );
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