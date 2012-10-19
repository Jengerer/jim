#include "item_schema.hpp"
#include <jui/application/error_stack.hpp>

/*
 * Item schema constructor.
 */
ItemSchema::ItemSchema( void )
{
    fallback_ = nullptr;
    set_loaded( false );
}

/*
 * Item schema destructor.
 */
ItemSchema::~ItemSchema( void )
{
    clear_definitions();

    // Destroy fallback.
    JUTIL::BaseAllocator::safe_destroy( &fallback_ );
}

/*
 * Add item definition to schema.
 */
bool ItemSchema::add_item_definition( uint16 index, ItemDefinition* item_definition )
{
    if (!item_definitions_.insert( index, item_definition )) {
        return false;
    }
    return true;
}

/*
 * Add attribute definition to schema.
 */
bool ItemSchema::add_attribute_definition( uint16 index, AttributeDefinition* attribute_definition )
{
    if (!attribute_definitions_.insert( index, attribute_definition )) {
        return false;
    }
    return true;
}

/*
 * Set definition to return when no definition found.
 */
void ItemSchema::set_fallback_definition( ItemDefinition* item_definition )
{
    fallback_ = item_definition;
}

/*
 * Clear all definitions from schema.
 */
void ItemSchema::clear_definitions( void )
{
    // Remove item definitions.
    ItemDefinitionMap::Iterator i;
    for (i = item_definitions_.begin(); i.has_next(); i.next()) {
        ItemDefinition* definition = i.get_value();
        JUTIL::BaseAllocator::destroy( definition );
    }
    item_definitions_.clear();

    // Remove attribute definitions.
    AttributeDefinitionMap::Iterator j;
    for (j = attribute_definitions_.begin(); j.has_next(); j.next()) {
        AttributeDefinition* definition = j.get_value();
        JUTIL::BaseAllocator::destroy( definition );
    }
    attribute_definitions_.clear();
}

/*
 * Get item definition for item index.
 * Returns current fallback if none found.
 */
const ItemDefinition* ItemSchema::get_item_definition( uint16 item_index ) const
{
    // Find item definition.
    ItemDefinition* definition;
    if (!item_definitions_.get( item_index, &definition )) {
        definition = fallback_;
    }
    return definition;
}

/*
 * Get attribute definition for attribute index.
 * Returns nullptr if none found.
 */
const AttributeDefinition* ItemSchema::get_attribute_definition( uint16 attribute_index ) const
{
    // Find attribute definition.
    AttributeDefinition* definition;
    if (!attribute_definitions_.get( attribute_index, &definition )) {
        definition = nullptr;
    }
    return definition;
}

/*
 * Resolve an item to its definition.
 */
bool ItemSchema::resolve_item( Item* item ) const
{
    // Check if already loaded.
    if (item->get_definition() != fallback_) {
        return true;
    }

    // Get definition for item.
    uint16 index = item->get_type_index();
    const ItemDefinition* definition = get_item_definition( index );
    item->set_definition( definition );

    // Update attributes.
    size_t i;
    size_t length = item->get_attribute_count();
    for (i = 0; i < length; ++i) {
        Attribute* attribute = item->get_attribute( i );
        if (!resolve_attribute( attribute )) {
            return false;
        }
    }
    return true;
}

/*
 * Resolve attribute to definition.
 */
bool ItemSchema::resolve_attribute( Attribute* attribute ) const
{
    // Error log if not found.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Check if already loaded.
    if (attribute->get_definition() != nullptr) {
        return true;
    }

    // Find definition.
    uint16 index = attribute->get_index();
    const AttributeDefinition* definition = get_attribute_definition( index );
    if (definition == nullptr) {
        stack->log( "Failed to find attribute definition for attribute index %u.", index );
        return false;
    }
    attribute->set_definition( definition );

    // Reload value if floating point.
	if (!definition->is_integer()) {
		AttributeValue value = attribute->get_value();
		float* float_value = reinterpret_cast<float*>(&value.as_uint32);
		value.as_float = *float_value;
		attribute->set_value( value );
	}

	// Generate description.
	if (!attribute->generate_description()) {
		stack->log( "Failed to generate description for attribute." );
		return false;
	}
    return true;
}