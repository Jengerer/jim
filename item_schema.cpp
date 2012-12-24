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
 * Set schema load state.
 */
void ItemSchema::set_loaded( bool is_loaded )
{
	is_loaded_ = is_loaded;
}

/*
 * Return whether schema has been fully loaded.
 */
bool ItemSchema::is_loaded( void ) const
{
	return is_loaded_;
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
 * Add quality name to schema.
 */
bool ItemSchema::add_quality_name( uint32 index, JUTIL::DynamicString* quality_name )
{
	if(!quality_names_.insert( index, quality_name )) {
		return false;
	}
	return true;
}

/*
 * Add origin name to schema.
 */
bool ItemSchema::add_origin_name( uint32 index, JUTIL::DynamicString* origin_name )
{
	if(!origin_names_.insert( index, origin_name )) {
		return false;
	}
	return true;
}

bool ItemSchema::add_kill_eater_type( uint32 index, KillEaterType* kill_eater_type )
{
	if(!kill_eater_types_.insert( index, kill_eater_type )) {
		return false;
	}
	return true;
}

bool ItemSchema::add_kill_eater_rank( const JUTIL::String* name, KillEaterRank* kill_eater_rank )
{
	if(!kill_eater_ranks_.insert( name, kill_eater_rank )) {
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

	// Remove quality names.
	QualityNameMap::Iterator k;
    for (k = quality_names_.begin(); k.has_next(); k.next()) {
        JUTIL::DynamicString* name = k.get_value();
        JUTIL::BaseAllocator::destroy( name );
    }
    quality_names_.clear();

	// Remove origin names.
	OriginNameMap::Iterator l;
    for (l = origin_names_.begin(); l.has_next(); l.next()) {
        JUTIL::DynamicString* name = l.get_value();
        JUTIL::BaseAllocator::destroy( name );
    }
    origin_names_.clear();
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
 * Get quality name for quality index.
 * Returns nullptr if none found.
 */
const JUTIL::DynamicString* ItemSchema::get_quality_name( uint32 quality_index ) const
{
	JUTIL::DynamicString* name;
	if(!quality_names_.get( quality_index, &name )) {
		name = nullptr;
	}
	return name;
}

/*
 * Get origin name for origin index.
 * Returns nullptr if none found.
 */
const JUTIL::DynamicString* ItemSchema::get_origin_name( uint32 origin_index ) const
{
	JUTIL::DynamicString* name;
	if(!origin_names_.get( origin_index, &name )) {
		name = nullptr;
	}
	return name;
}

const KillEaterType* ItemSchema::get_kill_eater_type( uint32 type_index ) const
{
	KillEaterType* type;
	if(!kill_eater_types_.get( type_index, &type )){
		type = nullptr;
	}
	return type;
}

const KillEaterRank* ItemSchema::get_kill_eater_rank( const JUTIL::String* name ) const
{
	KillEaterRank* rank;
	if(!kill_eater_ranks_.get( name, &rank )){
		rank = nullptr;
	}
	return rank;
}

/*
 * Resolve an item to its definition.
 */
bool ItemSchema::resolve( Item* item ) const
{
    // Get definition for item.
    uint16 index = item->get_type_index();
    const ItemDefinition* definition = get_item_definition( index );
    item->set_definition( definition );

	// Resolve all item attributes.
	size_t i;
	size_t length = item->get_attribute_count();
	for (i = 0; i < length; ++i) {
		Attribute* attribute = item->get_attribute( i );
		if (!resolve( attribute )) {
			return false;
		}
	}

	// Update item attributes.
	item->update_attributes();

	// Generate name for item if no custom.
	const JUTIL::String* name = item->get_name();
	if (name->get_length() == 0) {
		if (!item->generate_name()) {
			return false;
		}
	}
    return true;
}

/*
 * Resolve attribute to definition.
 */
bool ItemSchema::resolve( Attribute* attribute ) const
{
    // Error log if not found.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

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