#pragma once

#include "DirectX.h"

class Drawable
{
public:
	Drawable(const float xNew = 0.0f, const float yNew = 0.0f);

	// Pure virtual; override me.
	virtual void	drawObject(DirectX* pDirectX) = 0;

	// Position getter/setter.
	void			setPosition(const float xNew, const float yNew);

	// For input handling.
	bool			mouseTouching(MouseListener* pMouse) const;

	// Virtual size getters; return texture size by default.
	virtual int		getWidth() const;
	virtual int		getHeight() const;

	// Position member variables.
	float			m_fX, m_fY;

	// Drawing variables.
	int				m_nAlpha;

	// Class-wide texture.
	virtual const Texture*	getTexture() const;
	static Texture* m_lpTexture;
};