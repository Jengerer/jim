#ifndef ITEM_ATTRIBUTE_H
#define ITEM_ATTRIBUTE_H

#include "item_shared.hpp"

enum EffectType
{
	EFFECT_POSITIVE,
	EFFECT_NEGATIVE,
	EFFECT_NEUTRAL
};

class AttributeInformation
{

public:

	AttributeInformation( const std::string& name,
		unsigned int index,
		const std::string& attribute_class,
		float min_value,
		float max_value,
		EffectType effect_type,
		bool hidden,
		bool is_integer );

	// Accessing main attributes.
	const std::string&	get_name() const;
	unsigned int		get_index() const;
	const std::string&	get_attribute_class() const;
	float				get_min_value() const;
	float				get_max_value() const;
	EffectType			get_effect_type() const;
	bool				is_hidden() const;
	bool				is_integer() const;

	// Optional attributes.
	bool has_description() const;
	void set_description( const std::string& desc_string, const std::string& desc_format );
	const std::string& get_desc_string() const;
	const std::string& get_desc_format() const;

private:

	std::string name_;
	unsigned int index_;
	std::string attribute_class_;
	float min_value_;
	float max_value_;
	EffectType effect_type_;
	bool hidden_;
	bool is_integer_;

	// Optional members.
	bool has_description_;
	std::string desc_string_;
	std::string desc_format_;

};

#endif // ITEM_ATTRIBUTE_H