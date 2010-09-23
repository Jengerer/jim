#pragma once

#include "Drawable.h"
#include "Popup.h"

#define PADDING 20

class Dialog: public Popup
{
public:
	Dialog(const string& newMsg);

	// Overriding Drawable virtual functions.
	void	drawObject(DirectX* pDirectX);
	void	mouseInteract();
	int		getWidth();
	int		getHeight();

	void	setMessage(const string& newMsg);
	void	appendMessage(const string& addMsg);

	static Texture* m_lpTexture;

private:
	string	m_dialogMsg;
};