#pragma once

#include <d3d9.h>
#include "Exception.h"

class Window
{
public:
	//Class constructor.
	Window(HINSTANCE hInstance,
		WNDPROC wndProc, 
		char* newTitle,
		int newWidth, int newHeight);

	~Window();

	bool registerClass(HINSTANCE hInstance, WNDPROC wndProc);
	void createWindow(HINSTANCE hInstance);
	
	int getWidth() const;
	int getHeight() const;
	HWND getHandle() const;
private:
	HWND		m_hWnd;

	char*	m_strTitle;
	double	m_dVersion;
	int		m_iWidth, m_iHeight;
};