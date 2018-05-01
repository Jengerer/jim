#ifndef VDF_PARSER_HPP
#define VDF_PARSER_HPP

#include <containers/vector.hpp>
#include <string/constant_string.hpp>
#include <string/dynamic_string.hpp>

// Callback class for visiting VDF nodes.
class VdfVisitor
{
public:
	virtual void on_object_begin( const JUTIL::ConstantString& name ) = 0;
	virtual void on_object_end() = 0;
	virtual void on_property( const JUTIL::ConstantString& name, const JUTIL::ConstantString& value ) = 0;
};

class VdfParser
{
public:

	VdfParser( VdfVisitor& visitor );
	~VdfParser();

	// Start parsing a VDF body and make callbacks
	bool parse( const JUTIL::ConstantString& body );

private:

	bool parse_line( const JUTIL::ConstantString& line );

private:

	VdfVisitor& visitor_;

};

#endif // VDF_PARSER_HPP