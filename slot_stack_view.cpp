#include "slot_stack_view.hpp"
#include <jui/application/error_stack.hpp>

// Item selection parameters.
const float STACK_DRAGGING_ALPHA = 0.6f;
const float STACK_NORMAL_ALPHA = 0.0f;

// Selection count parameters.
const unsigned int SELECTION_COUNT_RADIUS = 3;
const unsigned int SELECTION_COUNT_PADDING = 5;
const unsigned int SELECTION_COUNT_OFFSET = 5;
const JUI::Colour SELECTION_COUNT_COLOUR = JUI::COLOUR_BLACK;
const JUTIL::ConstantString SELECTION_COUNT_TEXT_FONT( "fonts/tf2build.ttf" );
const unsigned int SELECTION_COUNT_TEXT_SIZE = 10;
const JUI::Colour SELECTION_COUNT_TEXT_COLOUR = JUI::COLOUR_WHITE;

SlotStackView::SlotStackView( SlotArrayInterface* slot_array )
    : slot_array_( slot_array ),
	  is_dragging_( false ),
	  count_font_( nullptr )
{
	set_alpha( STACK_NORMAL_ALPHA );
}

SlotStackView::~SlotStackView( void )
{
}

/*
 * Add the selection count to the layout.
 */
bool SlotStackView::initialize( void )
{
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Initialize base slot.
	if (!SlotView::initialize()) {
		return false;
	}

	// Create the count container.
	if (!JUTIL::BaseAllocator::allocate( &count_container_ )) {
		stack->log( "Failed to allocate selection count container." );
		return false;
	}
	if (!add( count_container_ )) {
		JUTIL::BaseAllocator::release( count_container_ );
		stack->log( "Failed to add selection count container." );
		return false;
	}
	new (count_container_) RoundedRectangleContainer( SELECTION_COUNT_PADDING );
	if (!count_container_->initialize()) {
		stack->log( "Failed to initialize selection count container." );
		return false;
	}
	RoundedRectangle* rectangle = count_container_->get_rounded_rectangle();
	rectangle->set_colour( &SELECTION_COUNT_COLOUR );
	rectangle->set_radius( SELECTION_COUNT_RADIUS );

	// Load the font here.
	count_font_ = JUI::FontFactory::create_font( &SELECTION_COUNT_TEXT_FONT, SELECTION_COUNT_TEXT_SIZE );
	if (count_font_ == nullptr) {
		stack->log( "Failed to create selection count font." );
		return false;
	}
	
	// Add the text element.
	if (!JUTIL::BaseAllocator::allocate( &count_text_ )) {
		stack->log( "Failed to allocate selection count text." );
		return false;
	}
	if (!count_container_->add( count_text_ )) {
		stack->log( "Failed to add selection count text." );
		return false;
	}
	new (count_text_) JUI::Text( count_font_ );
	count_text_->set_colour( &SELECTION_COUNT_TEXT_COLOUR );
	count_container_->set_content( count_text_ );
	count_container_->pack();
	return true;
}

/*
 * Set the listener interface for array view.
 */
void SlotStackView::set_listener( SlotArrayViewListener* listener )
{
	listener_ = listener;
}

/*
 * Handle slot container update event.
 */
void SlotStackView::on_slot_updated( unsigned int index, const Slot* slot )
{
    // Only update top slot.
    if (index == 0) {
		const Item* item = slot->get_item();
        update_item( item );
    }

	// Show selection count if we have more than one item.
	unsigned int count = slot_array_->get_size();
	float count_alpha;
	if (count <= 1) {
		count_alpha = 0.0f;
	}
	else {
		JUTIL::DynamicString extra;

		// Just don't bother if we fail to write.
		if (!extra.write( "+%d", (count - 1) )) {
			count_alpha = 0.0f;
		}
		else {
			count_text_->set_text( &extra );
			count_container_->pack();
			int offset_x = get_width() - count_container_->get_width() - SELECTION_COUNT_OFFSET;
			set_constraint( count_container_, offset_x, SELECTION_COUNT_OFFSET );
			count_alpha = 1.0f;
		}
	}
	count_container_->set_alpha( count_alpha );
}

/*
 * Set drag offset from mouse.
 */
void SlotStackView::set_drag_offset( int x, int y )
{
	offset_x_ = x;
	offset_y_ = y;
}

/*
 * Begin dragging the slot view.
 * Should be called after offset is set.
 */
void SlotStackView::begin_dragging( void )
{
	is_dragging_ = true;
	set_alpha( STACK_DRAGGING_ALPHA );
}

/*
 * End dragging the slot view.
 */
void SlotStackView::end_dragging( void )
{
	is_dragging_ = false;
	set_alpha( STACK_NORMAL_ALPHA );
}

/*
 * Return whether stack is enabled.
 */
bool SlotStackView::is_dragging( void ) const
{
	return is_dragging_;
}

/*
 * Update drag position based on mouse position.
 */
JUI::IOResult SlotStackView::on_mouse_moved( JUI::Mouse* mouse )
{
	// Only update if we're dragging.
	if (is_dragging_) {
		int x = mouse->get_x() + offset_x_;
		int y = mouse->get_y() + offset_y_;
		set_position( x, y );
		return JUI::IO_RESULT_HANDLED;
	}
	return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Dummy mouse handler.
 */
JUI::IOResult SlotStackView::on_mouse_clicked( JUI::Mouse* mouse )
{
	// Handle drag start if non-empty.
	if (slot_array_->get_size() != 0) {
		if (mouse->is_touching( this )) {
			// Set offset and begin dragging.
			offset_x_ = get_x() - mouse->get_x();
			offset_y_ = get_y() - mouse->get_y();
			set_drag_offset( offset_x_, offset_y_ );
			begin_dragging();
			
			// Notify listener we've been clicked.
			if (!listener_->on_slot_clicked( mouse, slot_array_, 0 )) {
				return JUI::IO_RESULT_ERROR;
			}
			return JUI::IO_RESULT_HANDLED;
		}
	}

	return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Dummy mouse handler.
 */
JUI::IOResult SlotStackView::on_mouse_released( JUI::Mouse* mouse )
{
	if (is_dragging_) {
		end_dragging();
		return JUI::IO_RESULT_HANDLED;
	}
	return JUI::IO_RESULT_UNHANDLED;
}