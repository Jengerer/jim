#pragma once

#include "Drawable.h"

class Button: public Drawable
{
public:
	Button(const string& newCaption);
	Button(const string& newCaption, const float xNew, const float yNew);
	
	// Override Drawable virtual functions.
	void	drawObject(DirectX* pDirectX);

	// Class-wide texture.
	virtual const Texture* getTexture() const;
	static Texture* m_lpTexture;

private:
	string	m_captionString;
};