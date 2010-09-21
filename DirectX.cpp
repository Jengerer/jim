#include "DirectX.h"

DirectX::DirectX(char* newTitle,
		HINSTANCE hInstance,
		int newWidth,
		int newHeight) : Main(newTitle, hInstance, newWidth, newHeight)
{
	try
	{
		loadInterfaces();
	} catch (Exception directException)
	{
		closeInterfaces();
		throw directException;
	}
}

DirectX::~DirectX()
{
	//DirectX has been destroyed.
	closeInterfaces();
}

void DirectX::loadInterfaces()
{
	m_lpDirect3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (m_lpDirect3D == NULL)
		throw Exception("Failed to create Direct3D.");

	ZeroMemory(&m_d3dParams, sizeof(m_d3dParams));

	m_d3dParams.Windowed			= TRUE;
	m_d3dParams.BackBufferCount		= 1;
	m_d3dParams.BackBufferFormat	= D3DFMT_X8R8G8B8;
	m_d3dParams.BackBufferWidth		= m_pWindow->getWidth();
	m_d3dParams.BackBufferHeight	= m_pWindow->getHeight();
	m_d3dParams.SwapEffect			= D3DSWAPEFFECT_DISCARD;
	m_d3dParams.hDeviceWindow		= m_pWindow->getHandle();

	m_lpDirect3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_pWindow->getHandle(),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&m_d3dParams,
		&m_lpDevice);

	if (m_lpDevice == NULL)
		throw Exception("Failed to create Direct3D device!");

	HRESULT hResult;
	hResult = D3DXCreateSprite(m_lpDevice, &m_lpSprite);
	if (hResult != D3D_OK)
		throw Exception("Failed to create sprite handler.");

	/* Load font for body text. */
	hResult = D3DXCreateFont(
		m_lpDevice,
		12, 0,
		FW_NORMAL,
		0,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		"Arial",
		&m_lpBody);

	if (hResult != D3D_OK)
		throw Exception("Failed to create font device.");

	/* Create an empty Hashtable. */
	m_pTextures = new Hashtable();

	/* Now attempt to load any textures. */
	loadTextures();
}

void DirectX::closeInterfaces()
{
	/* Delete the Hashtable of textures. */
	if (m_pTextures != NULL)
	{
		delete m_pTextures;
		m_pTextures = NULL;
	}

	/* Delete fonts. */
	if (m_lpBody != NULL)
	{
		m_lpBody->Release();
		m_lpBody = NULL;
	}

	/* Delete Sprite handler. */
	if (m_lpSprite)
	{
		m_lpSprite->Release();
		m_lpSprite = NULL;
	}

	/* Delete Device. */
	if (m_lpDevice)
	{
		m_lpDevice->Release();
		m_lpDevice = NULL;
	}

	/* Delete Direct3D. */
	if (m_lpDirect3D)
	{
		m_lpDirect3D->Release();
		m_lpDirect3D = NULL;
	}
}

void DirectX::loadTextures()
{
	/* Reload any existing unloaded textures. */
	stringAnyMap::iterator hashIterator;
	for (hashIterator = m_pTextures->begin(); hashIterator != m_pTextures->end(); hashIterator++)
	{
		try
		{
			Texture* thisTexture = boost::any_cast<Texture*>(hashIterator->second);
			if (!thisTexture->isLoaded())
			{
				const string* textureFile = thisTexture->getFilename();
				const string* textureURL = thisTexture->getURL();

				/* Load and set the xture. */
				Texture* tempTexture = loadTexture(textureFile, textureURL);
				thisTexture->setTexture(tempTexture->getTexture(), textureFile, textureURL);
			}
		} catch (const boost::bad_any_cast &)
		{
			throw Exception("Failed to get texture string, unexpected variable type received.");
		}
	}
}

void DirectX::releaseTextures()
{
	/* Get textures and release them. */
	stringAnyMap::iterator hashIterator;
	for (hashIterator = m_pTextures->begin(); hashIterator != m_pTextures->end(); hashIterator++)
	{
		// Just release it.
		try
		{
			Texture* thisTexture = boost::any_cast<Texture*>(hashIterator->second);
			thisTexture->releaseTexture();
		} catch (const boost::bad_any_cast &)
		{
			throw Exception("Failed to get texture from table, unexpected variable type received.");
		}
	}
}

//TODO: Make sure all calls check for success.
Texture* DirectX::getTexture(const string* textureName, const string* textureURL)
{
	//Check if that texture exists already.
	stringAnyMap::iterator hashIterator = m_pTextures->find(*textureName);
	if (hashIterator != m_pTextures->end())
	{
		try
		{
			return boost::any_cast<Texture*>(hashIterator->second);
		} catch (const boost::bad_any_cast &)
		{
			throw Exception("Failed to cast texture, unexpected variable type received.");
		}
	}

	// Otherwise, load the texture.
	Texture* destTexture = loadTexture(textureName, textureURL);
	
	// Now add it to dictionary.
	m_pTextures->put(*textureName, destTexture);

	// Return it.
	return destTexture;
}

Window* DirectX::getWindow()
{
	return m_pWindow;
}

int DirectX::getWidth() const
{
	return m_pWindow->getWidth();
}

int DirectX::getHeight() const
{
	return m_pWindow->getHeight();
}

//TODO: Make sure all calls check for failure.
Texture* DirectX::loadTexture(const string* textureName, const string* textureURL)
{
	LPDIRECT3DTEXTURE9	lpTexture;
	D3DXIMAGE_INFO		imgInfo;

	string filePath = "imgFiles/" + *textureName + ".png";
	HRESULT hResult;
	
	hResult = D3DXGetImageInfoFromFile(filePath.c_str(), &imgInfo);
	if (hResult != D3D_OK)
	{
		// Make sure image directory exists.
		if (GetFileAttributes("imgFiles") == INVALID_FILE_ATTRIBUTES)
			CreateDirectory("imgFiles", NULL);

		// Try to download the texture.
		if (!downloadFile(*textureURL, filePath))
		{
			// Try the mirror.
			string mirrorURL = "http://www.jengerer.com/itemManager/imgFiles/" + *textureName + ".png";
			if (!downloadFile(mirrorURL, filePath))
			{
				string errorMessage = "Failed to load texture:\n" + filePath;
				throw Exception(errorMessage);
			}
		}

		hResult = D3DXGetImageInfoFromFile(filePath.c_str(), &imgInfo);
		// If downloading succeeded, reload the texture.
		if (hResult != D3D_OK)
			
		{
			string errorMessage = "Failed to load texture:\n" + filePath;
			throw Exception(errorMessage);
		}
	}

	//Load the texture.
	hResult = D3DXCreateTextureFromFileEx(
		m_lpDevice,
		filePath.c_str(),
		imgInfo.Width, //Use file size.
		imgInfo.Height, //Use file size.
		1, 0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		NULL, NULL,
		&lpTexture);

	if (hResult != D3D_OK)
	{
		string errorMessage = "Failed to load texture:\n" + filePath;
		throw Exception(errorMessage);
	}

	/* If texture exists, fill it in. Otherwise, return new. */
	return new Texture(lpTexture, textureName, textureURL, imgInfo);
}

bool DirectX::beginDraw()
{
	/* Ensure we have all interfaces functional. */
	if (!checkDevice())
		return false;

	/* Clear background. */
	m_lpDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(43, 39, 37), 1.0, 0);

	/* Begin scene. */
	if (!m_lpDevice->BeginScene())
		return false;

	/* Begin drawing with sprite. */
	m_lpSprite->Begin(D3DXSPRITE_ALPHABLEND);
	return true;
}

void DirectX::endDraw()
{
	/* End drawing with sprite. */
	m_lpSprite->End();

	/* End scene. */
	m_lpDevice->EndScene();

	/* Present scene. */
	m_lpDevice->Present(NULL, NULL, NULL, NULL);
}

void DirectX::redrawScreen()
{
	// Start redraw.
	if (beginDraw())
	{
		onRedraw();
		endDraw();
	}
}

void DirectX::drawText(string whichText, RECT *whichPosition)
{
	m_lpBody->DrawTextA(m_lpSprite, whichText.c_str(), -1, whichPosition, 0, D3DCOLOR_RGBA(255, 255, 255, 100));
}

void DirectX::drawTexture(Texture* whichTexture, double xPosition, double yPosition)
{
	/* Get the texture. */
	LPDIRECT3DTEXTURE9 lpTexture = whichTexture->getTexture();
	
	/* Create the position object. */
	D3DXVECTOR3 posTexture(
		(float)xPosition,
		(float)yPosition,
		0.0f);

	// Get center.
	D3DXVECTOR3 centerTexture(
		(float)whichTexture->getWidth()/2,
		(float)whichTexture->getHeight()/2,
		0.0f);

	/* Now draw it. */
	m_lpSprite->Draw(
		whichTexture->getTexture(),
		NULL, &centerTexture,
		&posTexture,
		D3DCOLOR_XRGB(255, 255, 255));
}

bool DirectX::checkDevice()
{
	switch (m_lpDevice->TestCooperativeLevel())
	{
	case D3DERR_DEVICELOST:
		return false;
	case D3DERR_DEVICENOTRESET:
		{
			/* Release all the textures. */
			releaseTextures();

			/* Refresh the sprite. */
			m_lpSprite->OnLostDevice();
			HRESULT hResult = m_lpDevice->Reset(&m_d3dParams);
			m_lpSprite->OnResetDevice();

			/* Check that everything was properly refreshed. */
			if (hResult != D3D_OK)
			{
				MessageBox(NULL, "Failed to reset Direct3D device!", "Direct3D Device Failure", MB_OK);
				PostMessage(m_pWindow->getHandle(), WM_DESTROY, 0, 0);
			}

			/* Now load all the textures again. */
			loadTextures();
		}

		return false;
	default:
		/* Everything's fine. */
		return true;
	}
}

void DirectX::setTransform(const D3DXMATRIX *lpSprite)
{
	m_lpSprite->SetTransform(lpSprite);
}

void DirectX::setTransform(float xPos, float yPos, float numRadians, float xScale, float yScale)
{
	// Result matrix.
	D3DXMATRIX mSprite;
	D3DXVECTOR2 vecTranslation(0.0f, 0.0f);
	D3DXVECTOR2 vecCenter(xPos, yPos);
	D3DXVECTOR2 vecScale(xScale, yScale);
	D3DXMatrixTransformation2D(
		// Output variable.
		&mSprite, 

		// Scaling arguments.
		&vecCenter, 
		1.0f,
		&vecScale,

		// Rotation arguments
		&vecCenter,
		numRadians,

		// Translation arguments.
		&vecTranslation);		

	// Set it.
	setTransform(&mSprite);
}