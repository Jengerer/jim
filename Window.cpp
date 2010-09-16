#include "Window.h"

Window::Window(HINSTANCE hInstance,
			   WNDPROC wndProc,
			   char* newTitle, 
			   int newWidth, int newHeight)
{
	m_strTitle = newTitle;
	m_iWidth = newWidth;
	m_iHeight = newHeight;

	if (!registerClass(hInstance, wndProc)) 
		throw Exception("Failed to register class.");

	createWindow(hInstance);
}

Window::~Window()
{
	//Window has been destroyed.
}

bool Window::registerClass(HINSTANCE hInstance, WNDPROC wndProc)
{
	WNDCLASSEX wndCls;

	wndCls.hInstance		= hInstance;
	wndCls.hIcon			= NULL;
	wndCls.hIconSm			= NULL;
	wndCls.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wndCls.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);

	wndCls.cbSize			= sizeof(WNDCLASSEX);
	wndCls.cbWndExtra		= 0;
	wndCls.cbClsExtra		= 0;

	wndCls.lpfnWndProc		= wndProc;
	wndCls.lpszClassName	= m_strTitle;
	wndCls.lpszMenuName		= NULL;

	wndCls.style			= NULL;

	if (!RegisterClassEx(&wndCls))
	{
		MessageBox(NULL, "Failed to create window class!", "Window Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

void Window::createWindow(HINSTANCE hInstance)
{
	// Get screen resolution to center window.
	RECT rectScreen;
	GetWindowRect(GetDesktopWindow(), &rectScreen);

	int xWindow = (rectScreen.right/2)-(getWidth()/2);
	int yWindow = (rectScreen.bottom/2)-(getHeight()/2);

	/* Adjust window dimensions based on style. */
	RECT rectWindow;
	rectWindow.left = 0;
	rectWindow.top = 0;
	rectWindow.right = getWidth();
	rectWindow.bottom = getHeight();

	BOOL windowAdjust = AdjustWindowRect(&rectWindow, 
		WS_VISIBLE | WS_CAPTION | WS_DLGFRAME | WS_MINIMIZEBOX | WS_SYSMENU, 
		false);

	if (!windowAdjust)
		throw Exception("Failed to adjust window rectangle.");

	/* Create the window. */
	m_hWnd = CreateWindowA(
		m_strTitle,
		m_strTitle,
		WS_VISIBLE | WS_CAPTION | WS_DLGFRAME | WS_MINIMIZEBOX | WS_OVERLAPPED | WS_SYSMENU,
		xWindow, yWindow,
		rectWindow.right - rectWindow.left, rectWindow.bottom - rectWindow.top,
		NULL, NULL,
		hInstance,
		NULL);

	if (!m_hWnd)
		throw Exception("Failed to create window.");

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(m_hWnd);
}

int Window::getWidth() const
{
	return m_iWidth;
}

int Window::getHeight() const
{
	return m_iHeight;
}

HWND Window::getHandle() const
{
	return m_hWnd;
}