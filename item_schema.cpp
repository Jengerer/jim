#include "item_schema.hpp"
#include <jui/application/error_stack.hpp>

/*
 * Item schema constructor.
 */
ItemSchema::ItemSchema( void )
{
	set_loaded( false );
    fallback_ = nullptr;
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
 * Return whether schema has been loaded.
 */
bool ItemSchema::is_loaded( void ) const
{
	return is_loaded_;
}

/*
 * Change whether the schema has been loaded.
 */
void ItemSchema::set_loaded( bool is_loaded )
{
	is_loaded_ = is_loaded;
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
	if (!origin_names_.insert( index, origin_name )) {
		return false;
	}
	return true;
}

bool ItemSchema::add_kill_eater_type( uint32 index, KillEaterType* kill_eater_type )
{
	if (!kill_eater_types_.insert( index, kill_eater_type )) {
		return false;
	}
	return true;
}

bool ItemSchema::add_kill_eater_ranks( const JUTIL::String* name, KillEaterRanks* kill_eater_rank )
{
	if (!kill_eater_ranks_.insert( name, kill_eater_rank )) {
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

	// Remove strange types.
	KillEaterTypeMap::Iterator m;
	for (m = kill_eater_types_.begin(); m.has_next(); m.next()) {
        KillEaterType* type = m.get_value();
        JUTIL::BaseAllocator::destroy( type );
    }
	kill_eater_types_.clear();

	// Remove strange ranks.
	KillEaterRankMap::Iterator n;
    for (n = kill_eater_ranks_.begin(); n.has_next(); n.next()) {
		KillEaterRanks* rank = n.get_value();
        JUTIL::BaseAllocator::destroy( rank );
    }
	kill_eater_ranks_.clear();
}

/*
 * Get item definition for item index.
 * Returns current fallback if none found.
 */
ItemDefinition* ItemSchema::get_item_definition( uint16 item_index ) const
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
	if (!origin_names_.get( origin_index, &name )) {
		name = nullptr;
	}
	return name;
}

const KillEaterType* ItemSchema::get_kill_eater_type( uint32 type_index ) const
{
	KillEaterType* type;
	if (!kill_eater_types_.get( type_index, &type )){
		type = nullptr;
	}
	return type;
}

const KillEaterRanks* ItemSchema::get_kill_eater_ranks( const JUTIL::String* name ) const
{
	KillEaterRanks* rank;
	if (!kill_eater_ranks_.get( name, &rank )){
		rank = nullptr;
	}
	return rank;
}

/*
 * Resolve an item to its definition.
 */
bool ItemSchema::resolve( Item* item, JUI::Graphics2D *graphics ) const
{
	// Stack for reporting errors.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Get definition for item.
    uint16 index = item->get_type_index();
    ItemDefinition* definition = get_item_definition( index );

	// Load texture if necessary.
	const JUI::Texture* fallback = fallback_->get_texture();
	definition->load_texture( graphics, fallback );
    item->set_definition( definition );

	// Resolve all item attributes.
	size_t i;
	size_t length = item->get_local_attribute_count();
	for (i = 0; i < length; ++i) {
		Attribute* attribute = item->get_local_attribute( i );
		if (!resolve( attribute )) {
			stack->log( "Failed to resolve attribute with type index %u.\n", attribute->get_index() );
			return false;
		}
	}

	// Update item attributes.
	item->update_attributes();

	// Generate name for item.
	const JUTIL::ConstantString CUSTOM_NAME_ATTRIBUTE( "custom name attr" );
	const unsigned int CUSTOM_NAME_SIZE_OFFSET = 1;
	const unsigned int CUSTOM_NAME_STRING_OFFSET = 2;
	const Attribute* custom_name = item->find_attribute( &CUSTOM_NAME_ATTRIBUTE );
	if (custom_name) {
		// Length is written into second byte.
		const char* custom_name_value = custom_name->get_value();
		unsigned int size = static_cast<unsigned int>(custom_name_value[CUSTOM_NAME_SIZE_OFFSET]);

		// Write into dynamic string so we can get a null character.
		JUTIL::DynamicString name_buffer;
		if (!name_buffer.copy( custom_name_value + CUSTOM_NAME_STRING_OFFSET, size )) {
			stack->log( "Failed to write copy of custom name." );
			return false;
		}
		if (!item->set_custom_name( &name_buffer )) {
			JUTIL::BaseAllocator::destroy( item );
			stack->log( "Failed to set custom name for item." );
			return false;
		}
	}
	else {
		if (!item->generate_name()) {
			stack->log( "Failed to generate name string for item." );
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

	// Generate description.
	if (!attribute->generate_description()) {
		stack->log( "Failed to generate description for attribute." );
		return false;
	}
    return true;
}