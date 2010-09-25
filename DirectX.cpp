#include "DirectX.h"

DirectX::DirectX(const char* title,
		HINSTANCE hInstance,
		int width,
		int height) : Main( title, hInstance, width, height )
{
	try
	{
		openInterfaces();
	} catch (Exception dxException)
	{
		closeInterfaces();
		throw dxException;
	}
}

DirectX::~DirectX()
{
	//DirectX has been destroyed.
	closeInterfaces();
}

void DirectX::openInterfaces()
{
	d3d_ = Direct3DCreate9( D3D_SDK_VERSION );

	if (d3d_ == 0)
		throw Exception( "Failed to create Direct3D." );

	ZeroMemory(&params_, sizeof(params_));

	params_.Windowed			= TRUE;
	params_.BackBufferCount		= 1;
	params_.BackBufferFormat	= D3DFMT_X8R8G8B8;
	params_.BackBufferWidth		= window_->getWidth();
	params_.BackBufferHeight	= window_->getHeight();
	params_.SwapEffect			= D3DSWAPEFFECT_DISCARD;
	params_.hDeviceWindow		= window_->getHandle();

	d3d_->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window_->getHandle(),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&params_,
		&d3dDevice_);

	if (d3dDevice_ == 0)
		throw Exception( "Failed to create Direct3D device!" );

	HRESULT hResult;
	hResult = D3DXCreateSprite( d3dDevice_, &sprite_ );
	if (hResult != D3D_OK)
		throw Exception( "Failed to create sprite handler." );

	// Get font for body text.
	hResult = D3DXCreateFont(
		d3dDevice_,
		14, 0,
		FW_NORMAL,
		0,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		"Arial",
		&bodyFont_);

	if (hResult != D3D_OK)
		throw Exception( "Failed to create font device." );

	// Create empty hashtable.
	textureMap_ = new Hashtable();

	// Attempt to load any textures.
	loadTextures();
}

void DirectX::closeInterfaces()
{
	// Delete map of vectors.
	if (textureMap_ != 0)
	{
		delete textureMap_;
		textureMap_ = 0;
	}

	/* Delete fonts. */
	if (bodyFont_ != NULL)
	{
		bodyFont_->Release();
		bodyFont_ = 0;
	}

	/* Delete Sprite handler. */
	if (sprite_)
	{
		sprite_->Release();
		sprite_ = 0;
	}

	/* Delete Device. */
	if (d3dDevice_)
	{
		d3dDevice_->Release();
		d3dDevice_ = NULL;
	}

	/* Delete Direct3D. */
	if (d3d_)
	{
		d3d_->Release();
		d3d_ = NULL;
	}
}

void DirectX::loadTextures()
{
	/* Reload any existing unloaded textures. */
	stringMap::iterator i;
	for (i = textureMap_->begin(); i != textureMap_->end(); i++)
	{
		boost::any& value = i->second;
		try
		{
			Texture* texture = boost::any_cast<Texture*>(value);
			if (!texture->isLoaded())
			{
				const string& filePath = texture->getFilename();

				// Load and set the texture.
				Texture* tempTexture = loadTexture( filePath );
				texture->setTexture( tempTexture->getTexture(), filePath );
			}
		} catch (const boost::bad_any_cast &)
		{
			throw Exception( "Failed to get texture string, unexpected variable type received." );
		}
	}
}

void DirectX::releaseTextures()
{
	/* Get textures and release them. */
	stringMap::iterator hashIterator;
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
Texture* DirectX::getTexture(const string& textureName)
{
	//Check if that texture exists already.
	stringMap::iterator hashIterator = m_pTextures->find(textureName);
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
	Texture* destTexture = loadTexture(textureName);
	
	// Now add it to dictionary.
	m_pTextures->put(textureName, destTexture);

	// Return it.
	return destTexture;
}

Window* DirectX::getWindow()
{
	return window_;
}

int DirectX::getWidth() const
{
	return window_->getWidth();
}

int DirectX::getHeight() const
{
	return window_->getHeight();
}

//TODO: Make sure all calls check for failure.
Texture* DirectX::loadTexture(const string& textureName)
{
	LPDIRECT3DTEXTURE9	lpTexture;
	D3DXIMAGE_INFO		imgInfo;

	string filePath = "imgFiles/" + textureName + ".png";
	HRESULT hResult;
	
	hResult = D3DXGetImageInfoFromFile(filePath.c_str(), &imgInfo);
	if (hResult != D3D_OK)
	{
		// Make sure image directory exists.
		if (GetFileAttributes("imgFiles") == INVALID_FILE_ATTRIBUTES)
			CreateDirectory("imgFiles", NULL);

		// Try to download the texture.
		string fileURL = "http://www.jengerer.com/itemManager/" + filePath;
		if (!downloadFile(fileURL, filePath))
		{
			// Try the mirror.
			string errorMessage = "Failed to load texture:\n" + filePath;
			throw Exception(errorMessage);
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
		d3dDevice_,
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
	return new Texture(lpTexture, textureName, imgInfo);
}

bool DirectX::beginDraw()
{
	// Ensure we have all interfaces functional.
	if (!checkDevice())
		return false;

	// Clear background.
	d3dDevice_->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 43, 39, 37), 1.0, 0);

	// Begin scene.
	HRESULT hResult = d3dDevice_->BeginScene();
	if (hResult != D3D_OK) return false;

	// Begin drawing with sprite.
	sprite_->Begin(D3DXSPRITE_ALPHABLEND);
	return true;
}

void DirectX::endDraw()
{
	/* End drawing with sprite. */
	sprite_->End();

	/* End scene. */
	d3dDevice_->EndScene();

	/* Present scene. */
	d3dDevice_->Present(NULL, NULL, NULL, NULL);
}

void DirectX::redrawScreen()
{
	// Start redraw.
	if (beginDraw())
	{
		onRedraw();
		endDraw();
	} else
	{
		throw Exception("Failed to begin drawing.");
	}
}

void DirectX::drawText(const string& whichText, RECT *whichPosition, const DWORD& textFormat, const D3DCOLOR& whichColour)
{
	m_lpBody->DrawTextA(sprite_, whichText.c_str(), -1, whichPosition, textFormat, whichColour);
}

void DirectX::drawTexture(Texture* whichTexture, const float xPosition, const float yPosition)
{
	// Call other function with white colour.
	drawTexture(whichTexture, xPosition, yPosition, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void DirectX::drawTexture(Texture* whichTexture, const float xPosition, const float yPosition, const D3DCOLOR& whichColour)
{
	/* Get the texture. */
	LPDIRECT3DTEXTURE9 lpTexture = whichTexture->getTexture();
	
	/* Create the position object. */
	D3DXVECTOR3 posTexture(
		xPosition,
		yPosition,
		0.0f);

	/* Now draw it. */
	sprite_->Draw(
		whichTexture->getTexture(),
		NULL, NULL,
		&posTexture,
		whichColour);
}

bool DirectX::checkDevice()
{
	switch (d3dDevice_->TestCooperativeLevel())
	{
	case D3DERR_DEVICELOST:
		return false;
	case D3DERR_DEVICENOTRESET:
		{
			/* Release all the textures. */
			releaseTextures();

			/* Refresh the sprite. */
			sprite_->OnLostDevice();
			HRESULT hResult = d3dDevice_->Reset(&params_);
			sprite_->OnResetDevice();

			/* Check that everything was properly refreshed. */
			if (hResult != D3D_OK)
			{
				MessageBox(NULL, "Failed to reset Direct3D device!", "Direct3D Device Failure", MB_OK);
				PostMessage(window_->getHandle(), WM_DESTROY, 0, 0);
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
	sprite_->SetTransform(lpSprite);
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