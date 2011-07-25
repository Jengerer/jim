#ifndef FONT_FACTORY_H
#define FONT_FACTORY_H

#include "font.h"
#include <string>

class FontFactory
{

public:

	static void initialize();
	static void shut_down();

	static Font* create_font( const std::string& filename, FT_F26Dot6 height );

private:

	static FT_Library library_;

};

#endif // FONT_FACTOR_H