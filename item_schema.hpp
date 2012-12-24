#ifndef ITEM_SCHEMA_HPP
#define ITEM_SCHEMA_HPP

#include "attribute_definition.hpp"
#include "item_definition.hpp"
#include "item.hpp"
#include <containers/map.hpp>

// Definition structures.
typedef JUTIL::Map<uint16, ItemDefinition*> ItemDefinitionMap;
typedef JUTIL::Map<uint16, AttributeDefinition*> AttributeDefinitionMap;
typedef JUTIL::Map<uint32, JUTIL::DynamicString*> QualityNameMap;
typedef JUTIL::Map<uint32, JUTIL::DynamicString*> OriginNameMap;

/*
 * Structure for maintaining item and attribute definitions.
 */
class ItemSchema
{

public:

    // Construction.
    ItemSchema( void );
    ~ItemSchema( void );

    // Schema validation state.
    void set_loaded( bool is_loaded );
    bool is_loaded( void ) const;

    // Definition management.
    bool add_item_definition( uint16 index, ItemDefinition* item_definition );
    bool add_attribute_definition( uint16 index, AttributeDefinition* attribute_definition );
	bool add_quality_name( uint32 index, JUTIL::DynamicString* quality_name );
	bool add_origin_name( uint32 index, JUTIL::DynamicString* origin_name );

    void set_fallback_definition( ItemDefinition* item_definition );
    void clear_definitions( void );

    // Definition getters.
    const ItemDefinition* get_item_definition( uint16 item_index ) const;
    const AttributeDefinition* get_attribute_definition( uint16 attribute_index ) const;
	const JUTIL::DynamicString* get_quality_name( uint32 quality_index ) const;
	const JUTIL::DynamicString* get_origin_name( uint32 origin_index ) const;

    // Definition resolving after resources loaded.
    bool resolve( Item* item ) const;
    bool resolve( Attribute* attribute ) const;

private:

    // Valid state.
    bool is_loaded_;

    // Maps for definitions.
    ItemDefinitionMap item_definitions_;
    AttributeDefinitionMap attribute_definitions_;

	QualityNameMap quality_names_;
	OriginNameMap origin_names_;

    // Fallback definition for unmatched items.
    ItemDefinition* fallback_;

};

#endif // ITEM_SCHEMA_HPP