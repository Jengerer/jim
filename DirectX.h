#pragma once

#include "Main.h"
#include "Curl.h"
#include "Texture.h"
#include "Hashtable.h"
#include "Exception.h"

#include <string>
#include <sstream>

using namespace std;

class DirectX: public Main, public Curl
{
public:
	DirectX(char* newTitle,
		HINSTANCE hInstance,
		int newWidth,
		int newHeight);
	~DirectX();

	// Initializing and closing.
	void loadInterfaces();
	void closeInterfaces();

	// Resource handling.
	void loadTextures();
	void releaseTextures();
	Texture* getTexture(const string& textureName);
	Texture* loadTexture(const string& textureName);

	// Running functions.
	virtual void onRedraw() = 0;
	void redrawScreen();
	bool checkDevice();
	bool beginDraw();
	void endDraw();

	// Drawing functions.
	void drawTexture(Texture* whichTexture, float xPosition, float yPosition);
	void drawText(string whichText, RECT *whichPosition);	
	void setTransform(const D3DXMATRIX* lpMatrix);
	void setTransform(float xPos, float yPos, float numRadians, float xScale, float yScale);

	// Window handling.
	Window* getWindow();
	int getWidth() const;
	int getHeight() const;

private:
	// Direct3D interfaces.
	LPDIRECT3D9					m_lpDirect3D;
	LPDIRECT3DDEVICE9			m_lpDevice;
	LPD3DXSPRITE				m_lpSprite;

	// Text drawing.
	LPD3DXFONT					m_lpBody;

	// Present parameters.
	D3DPRESENT_PARAMETERS		m_d3dParams;

	// Texture handling.
	Hashtable*					m_pTextures;
};