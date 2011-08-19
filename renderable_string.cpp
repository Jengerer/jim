#include "renderable_string.h"

RenderableString::RenderableString( size_t length )
{
	length_ = length;
}

size_t RenderableString::length() const
{
	return length_;
}