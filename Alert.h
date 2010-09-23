#pragma once

#include "Dialog.h"
#include "Button.h"

class Alert: public Dialog
{
public:
	Alert(const string& newMsg);
	~Alert();

	void drawObject(DirectX* pDirectX);
	void mouseInteract();

private:
	Button* m_okayButton;
};