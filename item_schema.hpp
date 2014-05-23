#ifndef ITEM_SCHEMA_HPP
#define ITEM_SCHEMA_HPP

#include "attribute_definition.hpp"
#include "item_definition.hpp"
#include "kill_eater_type.hpp"
#include "kill_eater_ranks.hpp"
#include "item.hpp"
#include <containers/map.hpp>
#include <containers/hash_map.hpp>
#include <string/string_hasher.hpp>

// Definition structures.
typedef JUTIL::Map<uint16, ItemDefinition*> ItemDefinitionMap;
typedef JUTIL::Map<uint16, AttributeDefinition*> AttributeDefinitionMap;
typedef JUTIL::Map<uint32, JUTIL::DynamicString*> QualityNameMap;
typedef JUTIL::Map<uint32, JUTIL::DynamicString*> OriginNameMap;
typedef JUTIL::Map<uint32, KillEaterType*> KillEaterTypeMap;
typedef JUTIL::HashMap<unsigned int, const JUTIL::String*, KillEaterRanks*, JUTIL::StringHasher> KillEaterRankMap;

/*
 * Structure for maintaining item and attribute definitions.
 */
class ItemSchema
{

public:

    // Construction.
    ItemSchema( void );
    ~ItemSchema( void );

	// Schema state functions.
	bool is_loaded( void ) const;
	void set_loaded( bool is_loaded );

    // Definition management.
    bool add_item_definition( uint16 index, ItemDefinition* item_definition );
    bool add_attribute_definition( uint16 index, AttributeDefinition* attribute_definition );
	bool add_quality_name( uint32 index, JUTIL::DynamicString* quality_name );
	bool add_origin_name( uint32 index, JUTIL::DynamicString* origin_name );
	bool add_kill_eater_type( uint32 index, KillEaterType* kill_eater_type );
	bool add_kill_eater_ranks( const JUTIL::String* name, KillEaterRanks* kill_eater_rank );

    void set_fallback_definition( ItemDefinition* item_definition );
    void clear_definitions( void );

    // Definition getters.
    ItemDefinition* get_item_definition( uint16 item_index ) const;
    const AttributeDefinition* get_attribute_definition( uint16 attribute_index ) const;
	const JUTIL::DynamicString* get_quality_name( uint32 quality_index ) const;
	const JUTIL::DynamicString* get_origin_name( uint32 origin_index ) const;
	const KillEaterType* get_kill_eater_type( uint32 type_index ) const;
	const KillEaterRanks* get_kill_eater_ranks( const JUTIL::String* name ) const;

    // Definition resolving after resources loaded.
    bool resolve( Item* item, JUI::Graphics2D* graphics ) const;
    bool resolve( Attribute* attribute ) const;

private:

	// Schema state variables.
	bool is_loaded_;

    // Maps for definitions.
    ItemDefinitionMap item_definitions_;
    AttributeDefinitionMap attribute_definitions_;

	QualityNameMap quality_names_;
	OriginNameMap origin_names_;

	KillEaterTypeMap kill_eater_types_;
	KillEaterRankMap kill_eater_ranks_;

    // Fallback definition for unmatched items.
    ItemDefinition* fallback_;

};

#endif // ITEM_SCHEMA_HPP