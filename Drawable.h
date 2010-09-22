#pragma once

#include "DirectX.h"

class Drawable
{
public:
	Drawable();
	Drawable(const float xNew, const float yNew);

	// Pure virtual; override me.
	virtual void	drawObject(DirectX* pDirectX) = 0;

	// Pure virtuals for getting width; defined by child.
	virtual int		getWidth() = 0;
	virtual int		getHeight() = 0;

	// Position member variables.
	float			m_fX, m_fY;
};