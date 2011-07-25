#include "opengl_shared.h"

GLsizei next_power_of_2( GLsizei num ) {
	GLsizei result = 1;
	while (result < num) {
		result <<= 1;
	}

	return result;
}