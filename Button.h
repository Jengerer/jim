#pragma once

#include "Drawable.h"

class Button: public Drawable
{
public:
	Button(const string& newCaption);
	Button(const string& newCaption, const float xNew, const float yNew);
	
	void	drawObject(DirectX* pDirectX);
	int		getWidth();
	int		getHeight();

	static Texture* m_lpTexture;
private:
	string	m_captionString;
};