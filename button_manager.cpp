#include "button_manager.hpp"

ButtonManager::ButtonManager( ButtonEventListener* listener )
{
    listener_ = listener;
}

ButtonManager::~ButtonManager( void )
{
}

/*
 * Create and return a pointer to the new button.
 * Adds to the button manager after creation.
 */
Button* ButtonManager::create_button( const JUTIL::String* label, const JUI::Texture* icon )
{
    // Allocate new button.
    Button* button;
    if (!JUTIL::BaseAllocator::allocate( &button )) {
        return nullptr;
    }
    new (button) Button();
    if (!button->initialize( label, icon )) {
        JUTIL::BaseAllocator::destroy( button );
        return nullptr;
    }

    // Add button to vector.
    if (!buttons_.push( button )) {
        JUTIL::BaseAllocator::destroy( button );
        return nullptr;
    }
}