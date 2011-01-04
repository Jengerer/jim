#include "DirectX.h"

const D3DCOLOR WHITE_COLOUR = D3DCOLOR_ARGB( 255, 255, 255, 255 );
const D3DCOLOR BACKGROUND_COLOUR = D3DCOLOR_ARGB( 255, 43, 39, 37 );

DirectX::DirectX(const char* title,
		HINSTANCE hInstance,
		int width,
		int height) : Main( title, hInstance, width, height )
{
	// Set as null.
	textureMap_ = 0;

	try {
		openInterfaces();
	}
	catch (Exception dxException) {
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

	D3DDISPLAYMODE d3dMode;
	d3d_->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3dMode );

	ZeroMemory( &params_, sizeof( D3DPRESENT_PARAMETERS ) );

	params_.Windowed				= TRUE;
	params_.SwapEffect				= D3DSWAPEFFECT_DISCARD;
	params_.BackBufferFormat		= d3dMode.Format;

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

	DWORD shaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_DEBUG;
	hResult = D3DXCreateEffectFromFile( d3dDevice_, 
		"SampleShader.fx", 
		NULL, NULL, 
		shaderFlags, 
		NULL, 
		&d3dEffect_, 
		NULL );

	if (hResult != D3D_OK)
		throw Exception( "Failed to create effect from file." );

	// Create vertex buffer.
	hResult = d3dDevice_->CreateVertexBuffer(
		4 * sizeof(TextureVertex),
		D3DUSAGE_WRITEONLY,
		D3D9T_TEXTUREVERTEX,
		D3DPOOL_DEFAULT,
		&vertexBuffer_,
		NULL );

	d3dDevice_->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	if (hResult != D3D_OK)
		throw Exception( "Failed to create vertex buffer." );

	// Create empty hashtable.
	textureMap_ = new Hashtable();
}

void DirectX::closeInterfaces()
{
	// Delete map of vectors.
	if (textureMap_) {
		freeTextures();

		delete textureMap_;
		textureMap_ = 0;
	}

	// Delete fonts.
	if (bodyFont_) {
		bodyFont_->Release();
		bodyFont_ = 0;
	}

	// Delete sprite handler.
	if (sprite_) {
		sprite_->Release();
		sprite_ = 0;
	}

	// Delete Direct3D device.
	if (d3dDevice_) {
		d3dDevice_->Release();
		d3dDevice_ = 0;
	}

	// Delete Direct3D interface.
	if (d3d_) {
		d3d_->Release();
		d3d_ = 0;
	}
}

void DirectX::freeTextures()
{
	// Delete all texture objects.
	stringMap::iterator i;
	while (!textureMap_->empty()) {
		i = textureMap_->begin();

		try {
			Texture* thisTexture = boost::any_cast<Texture*>(i->second);

			// Delete the texture and remove it.
			delete thisTexture;
			textureMap_->remove(i);
		}
		catch (const boost::bad_any_cast &) {
			throw Exception( "Failed to get texture from table, unexpected variable type received." );
		}
	}
}

void DirectX::loadTextures()
{
	// Reload any existing unloaded textures.
	stringMap::iterator i;
	for (i = textureMap_->begin(); i != textureMap_->end(); i++) {
		boost::any& value = i->second;
		try {
			Texture* texture = boost::any_cast<Texture*>(value);
			if (!texture->isLoaded()) {
				const string& filePath = texture->getFilename();

				// Load and set the texture.
				Texture* tempTexture = loadTexture( filePath );
				texture->setTexture( tempTexture->getTexture(), filePath );
			}
		}
		catch (const boost::bad_any_cast &) {
			throw Exception( "Failed to get texture string, unexpected variable type received." );
		}
	}
}

void DirectX::releaseTextures()
{
	// Get textures and release them.
	stringMap::iterator i;
	for (i = textureMap_->begin(); i != textureMap_->end(); i++) {
		try {
			// Just release it.
			Texture* thisTexture = boost::any_cast<Texture*>(i->second);
			thisTexture->releaseTexture();
		}
		catch (const boost::bad_any_cast &) {
			throw Exception( "Failed to get texture from table, unexpected variable type received." );
		}
	}
}

// TODO: Make sure all calls check for success.
Texture* DirectX::getTexture( const string& filename )
{
	//Check if that texture exists already.
	stringMap::iterator iter = textureMap_->find( filename );
	if (iter != textureMap_->end()) {
		try {
			return boost::any_cast<Texture*>( iter->second );
		}
		catch (const boost::bad_any_cast &) {
			throw Exception( "Failed to cast texture, unexpected variable type received." );
		}
	}

	// Load texture, and add to map.
	Texture* dest = loadTexture( filename );
	textureMap_->put( filename, dest );

	// Return it.
	return dest;
}

//TODO: Make sure all calls check for failure.
Texture* DirectX::loadTexture( const string& filename )
{
	LPDIRECT3DTEXTURE9	texture;
	D3DXIMAGE_INFO		info;

	string path = "imgFiles/" + filename + ".png";
	HRESULT hResult;
	
	hResult = D3DXGetImageInfoFromFile( path.c_str(), &info );
	if (hResult != D3D_OK)
	{
		// Make sure image directory exists.
		if (GetFileAttributes( "imgFiles" ) == INVALID_FILE_ATTRIBUTES)
			CreateDirectory( "imgFiles", 0 );

		// Try to download the texture.
		string fileURL = "http://www.jengerer.com/itemManager/" + path;
		if (!download( fileURL, path )) {
			// Try the mirror.
			string errorMessage = "Failed to load texture:\n" + path;
			throw Exception( errorMessage );
		}

		hResult = D3DXGetImageInfoFromFile( path.c_str(), &info );
		// If downloading succeeded, reload the texture.
		if (hResult != D3D_OK) {
			string errorMessage = "Failed to load texture:\n" + path;
			throw Exception( errorMessage );
		}
	}

	// Load the texture.
	hResult = D3DXCreateTextureFromFileEx(
		d3dDevice_,
		path.c_str(),
		info.Width, // Use file size.
		info.Height, // Use file size.
		1, 0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		NULL, NULL,
		&texture);

	if (hResult != D3D_OK) {
		string errorMessage = "Failed to load texture:\n" + path;
		throw Exception( errorMessage );
	}

	// Return new texture.
	// TODO: Investigate whether there's a need for filling in texture.
	return new Texture( texture, filename, info );
}

void DirectX::createTexturedQuad( TextureVertex* vertices, float x, float y, int width, int height ) {
	// Set universals.
	for (int i = 0; i < 4; i++) {
		vertices[i].z = 1.0f;
		vertices[i].rhw = 0.5f;
	}

	// Set corners.
	TextureVertex *topLeft = &vertices[1];
	TextureVertex *bottomLeft = &vertices[0];
	TextureVertex *bottomRight = &vertices[2];
	TextureVertex *topRight = &vertices[3];

	/*x = (int)x;
	y = (int)y;*/
	
	// Top left.
	topLeft->x = x - 0.5;
	topLeft->y = y - 0.5;
	topLeft->tu = 0;
	topLeft->tv = 0;

	// Top right.
	topRight->x = x + (float)width - 0.5;
	topRight->y = y - 0.5;
	topRight->tu = 1;
	topRight->tv = 0;

	// Bottom right.
	bottomRight->x = x + (float)width - 0.5;
	bottomRight->y = y + (float)height - 0.5;
	bottomRight->tu = 1;
	bottomRight->tv = 1;

	// Bottom left.
	bottomLeft->x = x - 0.5;
	bottomLeft->y = y + (float)height - 0.5;
	bottomLeft->tu = 0;
	bottomLeft->tv = 1;
}

void DirectX::createColouredQuad( ColourVertex* vertices, float x, float y, int width, int height, DWORD colour ) {
	// Set universals.
	for (int i = 0; i < 4; i++) {
		vertices[i].z = 1.0f;
		vertices[i].rhw = 1.0f;
		vertices[i].colour = 0xFFFFFFFF;
	}

	// Set corners.
	ColourVertex *topLeft = &vertices[1];
	ColourVertex *bottomLeft = &vertices[0];
	ColourVertex *bottomRight = &vertices[2];
	ColourVertex *topRight = &vertices[3];

	/*x = (int)x;
	y = (int)y;*/
	
	// Top left.
	topLeft->x = x;
	topLeft->y = y;

	// Top right.
	topRight->x = x + (float)width;
	topRight->y = y;
	
	// Bottom right.
	bottomRight->x = x + (float)width;
	bottomRight->y = y + (float)height;

	// Bottom left.
	bottomLeft->x = x;
	bottomLeft->y = y + (float)height;
}

void DirectX::drawRoundedRect( float x, float y, int width, int height, float radius, DWORD colour ) {
	// Set effect.
	float centerX = 0.0f;
	float centerY = 0.0f;

	// Create and draw top-left.
	createColouredQuad( clrVertices_, x, y, radius, radius, colour );
	drawQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Create and draw top-right.
	createColouredQuad( clrVertices_, x + width - radius, y, radius, radius, colour );
	drawQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Create and draw bottom-right.
	createColouredQuad( clrVertices_, x + width - radius, y + height - radius, radius, radius, colour );
	drawQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Create and draw bottom-left.
	createColouredQuad( clrVertices_, x, y + height - radius, radius, radius, colour );
	drawQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Create and draw middle.
	createColouredQuad( clrVertices_, x, y + radius, width, height - 2 * radius, colour );
	drawQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Create and draw top.
	createColouredQuad( clrVertices_, x + radius, y, width - 2 * radius, radius, colour );
	drawQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Create and draw bottom.
	createColouredQuad( clrVertices_, x + radius, y + height - radius, width - 2 * radius, radius, colour );
	drawQuad( clrVertices_, sizeof( clrVertices_ ) );
}

void DirectX::drawQuad( void *vertices, size_t verticesSize )
{
	// Copy information and unlock.
	void *verticesResult;
	HRESULT hResult = vertexBuffer_->Lock( 0, 0, &verticesResult, 0 );
	if (hResult != D3D_OK) {
		throw Exception( "Failed to lock vertex buffer." );
	}

	memcpy( verticesResult, vertices, verticesSize );
	vertexBuffer_->Unlock();

	d3dDevice_->SetTexture( 0, NULL );
	d3dDevice_->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	d3dDevice_->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	d3dDevice_->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	d3dDevice_->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	d3dDevice_->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

	d3dDevice_->SetFVF( D3D9T_COLOURVERTEX );
	d3dDevice_->SetStreamSource( 0, vertexBuffer_, 0, sizeof( ColourVertex ) );
	d3dDevice_->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
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

bool DirectX::beginDraw()
{
	// Ensure we have all interfaces functional.
	if (!checkDevice())
		return false;

	// Clear background.
	d3dDevice_->Clear( 0, NULL, D3DCLEAR_TARGET, BACKGROUND_COLOUR, 1.0f, 0 );

	// Begin scene.
	HRESULT hResult = d3dDevice_->BeginScene();
	if (hResult != D3D_OK)
		return false;

	UINT passes;
	d3dEffect_->Begin( &passes, 0 );
	d3dEffect_->BeginPass( 0 );

	// Begin drawing with sprite.
	return true;
}

void DirectX::endDraw()
{
	d3dEffect_->EndPass();
	d3dEffect_->End();

	/* End scene. */
	d3dDevice_->EndScene();

	/* Present scene. */
	d3dDevice_->Present(NULL, NULL, NULL, NULL);
}

void DirectX::redraw()
{
	// Start redraw.
	if (beginDraw()) {
		onRedraw();
		endDraw();
	}
	else {
		throw Exception( "Failed to begin drawing." );
	}
}

void DirectX::drawText( const string& text, RECT *rect, const DWORD& format, const D3DCOLOR& colour )
{
	bodyFont_->DrawTextA( 0, text.c_str(), -1, rect, format, colour );
}

void DirectX::drawTexture( Texture* texture, float x, float y )
{
	// Call other function with white colour.
	drawTexture( texture, x, y, WHITE_COLOUR );
}

void DirectX::drawTexture( Texture* texture, const float x, const float y, const D3DCOLOR& colour)
{
	// Create quad.
	createTexturedQuad( texVertices_, x, y, texture->getWidth(), texture->getHeight() );

	// Add to vertex buffer.
	void *vertices = 0;
	HRESULT hr = vertexBuffer_->Lock( 0, 0, &vertices, 0 );
	memcpy( vertices, texVertices_, sizeof( texVertices_ ) );
	vertexBuffer_->Unlock();

	// Set alpha blending modes.
	d3dDevice_->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	d3dDevice_->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
	d3dDevice_->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	d3dDevice_->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// Draw quad.
	d3dDevice_->SetFVF( D3D9T_TEXTUREVERTEX );
	d3dDevice_->SetStreamSource( 0, vertexBuffer_, 0, sizeof( TextureVertex ) );
	d3dDevice_->SetTexture( 0, texture->getTexture() );
	d3dDevice_->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

bool DirectX::checkDevice()
{
	HRESULT hResult = d3dDevice_->TestCooperativeLevel();
	switch (hResult)
	{
	case D3DERR_DEVICELOST:
		return false;
	case D3DERR_DEVICENOTRESET:
		{
			// Release all textures.
			releaseTextures();

			// Refresh the sprite.
			sprite_->OnLostDevice();
			HRESULT hResult = d3dDevice_->Reset( &params_ );
			sprite_->OnResetDevice();

			// Check that everything was properly refreshed.
			if (hResult != D3D_OK)
			{
				throw Exception( "Failed to reset Direct3D device." );
				PostMessage( window_->getHandle(), WM_DESTROY, 0, 0 );
			}

			// Reload all textures.
			loadTextures();
		}

		return false;
	default:
		// Device is fine.
		return true;
	}
}

void DirectX::setTransform( const D3DXMATRIX *d3dMatrix )
{
	sprite_->SetTransform( d3dMatrix );
}

void DirectX::setTransform( float x, float y, float rotation, float xScale, float yScale)
{
	// Result matrix.
	D3DXMATRIX matrix;

	// Transform vectors.
	D3DXVECTOR2 vecTranslation( 0.0f, 0.0f );
	D3DXVECTOR2 vecCenter( x, y );
	D3DXVECTOR2 vecScale( xScale, yScale );

	D3DXMatrixTransformation2D(
		// Output variable.
		&matrix, 

		// Scaling arguments.
		&vecCenter, 
		1.0f,
		&vecScale,

		// Rotation arguments
		&vecCenter,
		rotation,

		// Translation arguments.
		&vecTranslation);		

	// Set it.
	setTransform(&matrix);
}