#include "vdf_parser.hpp"
#include <jui/application/error_stack.hpp>

VdfParser::VdfParser( VdfVisitor& visitor ) :
	visitor_( visitor )
{
}

VdfParser::~VdfParser()
{
}

bool VdfParser::parse( const JUTIL::ConstantString& body )
{
	const char* buffer = body.get_string();
	const char* line_start = buffer;
	const size_t length = body.get_length();
	const char* end = buffer + length;
	for (const char* iterator = buffer; iterator != end; ++iterator)
	{
		const char current = *iterator;
		switch (current) {
		case '\n':
		{
			const size_t line_length = static_cast<size_t>(iterator - line_start);
			const JUTIL::ConstantString line( line_start, line_length );
			if (!parse_line( line ))
			{
				return false;
			}
			line_start = iterator + 1;
			break;
		}

		default:
			break;
		}
	}

	return true;
}

bool VdfParser::parse_line( const JUTIL::ConstantString& line )
{
	// Prepare to dump error
	JUI::ErrorStack* error_stack = JUI::ErrorStack::get_instance();

	// Parsing iterators
	bool token_started = false;
	size_t token_count = 0;
	JUTIL::Vector<char> token_helper;
	JUTIL::Vector<JUTIL::ConstantString> tokens;
	
	// Go through the line, format is pretty simple
	const char* line_start = line.get_string();
	const size_t length = line.get_length();
	const char* line_end = line_start + length;
	for (const char* iterator = line_start; iterator != line_end; ++iterator)
	{
		const char current = *iterator;
		switch (current) {
			case '\\':
			{
				// Skip this character, add next unconditionally
				const char* next_iterator = iterator + 1;
				token_helper.push( *next_iterator );
				iterator = next_iterator;
				break;
			}

			case '\"':
			{
				// Start or commit token
				if (!token_started)
				{
					token_started = true;
					const size_t token_start_index = static_cast<size_t>(iterator - line_start);
					break;
				}
				else
				{
					// Add a null terminator
					token_helper.push( '\0' );
					token_started = false;
					++token_count;
				}
				break;
			}

			case ' ':
			case '\t':
			case '{':
			{
				// Skippable characters
				break;
			}

			case '}':
			{
				// End this object
				if (token_started || (token_count != 0))
				{
					error_stack->log( "Unexpected format: closing brace after tokens." );
					return false;
				}

				visitor_.on_object_end();
			}

			default:
			{
				// Should only encounter other characters inside a token
				if (!token_started)
				{
					error_stack->log( "Unexpected character '%c' found outside token.", current );
					return false;
				}
				break;
			}
		}
	}

	// Reached end of line, handle tokens
	const size_t object_name_token_count = 1;
	const size_t key_value_token_count = 2;
	switch (token_count)
	{
		case 0:
		{
			// Useless line
			break;
		}

		case object_name_token_count:
		{
			// Whole helper is the token
			const JUTIL::ConstantString object_name( token_helper.get_buffer(), token_helper.get_length() );
			visitor_.on_object_begin( object_name );
			break;
		}

		case key_value_token_count:
		{
			// Split into key/value
			const char* first_token_start = token_helper.get_buffer();
			const JUTIL::ConstantString key( first_token_start );
			const char* second_token_start = first_token_start + key.get_length();
			const JUTIL::ConstantString value( second_token_start );
			visitor_.on_property( key, value );
			break;
		}

		default:
		{
			error_stack->log( "Got %u tokens on one line!", static_cast<unsigned int>(token_count));
			return false;
		}
	}

	return true;
}
