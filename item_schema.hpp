#ifndef ITEM_SCHEMA_HPP
#define ITEM_SCHEMA_HPP

#include "attribute_definition.hpp"
#include "item_definition.hpp"
#include "item.hpp"
#include <containers/map.hpp>

// Definition structures.
typedef JUTIL::Map<uint16, ItemDefinition*> ItemDefinitionMap;
typedef JUTIL::Map<uint16, AttributeDefinition*> AttributeDefinitionMap;

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
    void set_fallback_definition( ItemDefinition* item_definition );
    void clear_definitions( void );

    // Definition getters.
    const ItemDefinition* get_item_definition( uint16 item_index ) const;
    const AttributeDefinition* get_attribute_definition( uint16 attribute_index ) const;

    // Definition resolving after resources loaded.
    bool resolve( Item* item ) const;
    bool resolve( Attribute* attribute ) const;

private:

    // Valid state.
    bool is_loaded_;

    // Maps for definitions.
    ItemDefinitionMap item_definitions_;
    AttributeDefinitionMap attribute_definitions_;

    // Fallback definition for unmatched items.
    ItemDefinition* fallback_;

};

#endif // ITEM_SCHEMA_HPP