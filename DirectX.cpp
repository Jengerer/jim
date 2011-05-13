#include "DirectX.h"

const D3DCOLOR WHITE_COLOUR = D3DCOLOR_ARGB( 255, 255, 255, 255 );
const D3DCOLOR BACKGROUND_COLOUR = D3DCOLOR_ARGB( 255, 43, 39, 37 );

DirectX::DirectX( HINSTANCE hInstance,
	const char *title,
	int width, int height ) : Window( hInstance, title, width, height )
{
	// Set as null.
	textureMap_ = 0;
	roundedCorner_ = 0;

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
		getHandle(),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&params_,
		&d3dDevice_);

	if (d3dDevice_ == 0)
		throw Exception( "Failed to create Direct3D device!" );

	HRESULT hResult;
	hResult = D3DXCreateSprite( d3dDevice_, &sprite_ );
	if (hResult != D3D_OK)
		throw Exception( "Failed to create sprite handler." );

	// Load font as resource.
	if (!AddFontResourceEx("ttfFiles/tf2Build.ttf", FR_PRIVATE, 0)) {
		if (!download("www.jengerer.com/itemManager/ttfFiles/tf2Build.ttf", "ttfFiles/tf2Build.ttf") || !AddFontResource("ttfFiles/tf2Build.ttf")) {
			throw Exception( "Failed to load font." );
		}
	}

	// Load font as resource.
	if (!AddFontResourceEx("ttfFiles/tf2Secondary.ttf", FR_PRIVATE, 0)) {
		if (!download("www.jengerer.com/itemManager/ttfFiles/tf2Secondary.ttf", "ttfFiles/tf2Secondary.ttf") || !AddFontResource("ttfFiles/tf2Secondary.ttf")) {
			throw Exception( "Failed to load font." );
		}
	}

	// Create vertex buffer.
	hResult = d3dDevice_->CreateVertexBuffer(
		4 * sizeof(TextureVertex),
		D3DUSAGE_WRITEONLY,
		D3D9T_TEXTUREVERTEX,
		D3DPOOL_DEFAULT,
		&vertexBuffer_,
		NULL );

	// Set render and stage states.
	d3dDevice_->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	// Set alpha blending modes.
	d3dDevice_->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	d3dDevice_->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
	d3dDevice_->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	d3dDevice_->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// First argument for diffuse quads.
	d3dDevice_->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	d3dDevice_->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

	// Second argument for texture quads.
	d3dDevice_->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	d3dDevice_->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	if (hResult != D3D_OK)
		throw Exception( "Failed to create vertex buffer." );

	// Create empty hashtable.
	textureMap_ = new Hashtable();

	// Load round corner.
	roundedCorner_ = getTexture( "manager/rounded_corner" );
}

void DirectX::closeInterfaces()
{
	// Delete map of vectors.
	if (textureMap_ != nullptr) {
		freeTextures();

		delete textureMap_;
		textureMap_ = nullptr;
	}

	// Remove font resource.
	RemoveFontResourceEx( "ttfFiles/tf2Build.ttf", FR_PRIVATE, 0 );
	RemoveFontResourceEx( "ttfFiles/tf2Secondary.ttf", FR_PRIVATE, 0 );

	// Delete sprite handler.
	// TODO: sprite handler is deprecated.
	if (sprite_ != nullptr) {
		sprite_->Release();
		sprite_ = nullptr;
	}

	// Delete Direct3D device.
	if (d3dDevice_ != nullptr) {
		d3dDevice_->Release();
		d3dDevice_ = nullptr;
	}

	// Delete Direct3D interface.
	if (d3d_ != nullptr) {
		d3d_->Release();
		d3d_ = nullptr;
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
			if (!texture->IsLoaded()) {
				LoadTexture( texture );
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
			thisTexture->ReleaseTexture();
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

	// Create the new texture and load it.
	Texture* newTexture = new Texture( filename );
	LoadTexture( newTexture );

	// Add to map and return.
	textureMap_->put( filename, newTexture );
	return newTexture;
}

void DirectX::LoadTexture( Texture *texture )
{
	// Don't load again.
	if ( texture->IsLoaded() ) {
		return;
	}

	IDirect3DTexture9	*d3dTexture;
	D3DXIMAGE_INFO		info;

	string path = "imgFiles/" + texture->GetTextureFilename() + ".png";
	HRESULT hResult = D3DXGetImageInfoFromFile( path.c_str(), &info );
	if (hResult != D3D_OK) {
		// Make sure image directory exists.
		if (GetFileAttributes( "imgFiles" ) == INVALID_FILE_ATTRIBUTES) {
			CreateDirectory( "imgFiles", 0 );
		}

		// Try to download the texture.
		string fileURL = "http://www.jengerer.com/itemManager/" + path;
		if (!download( fileURL, path )) {
			string errorMessage = "Failed to load texture:\n" + path;
			throw Exception( errorMessage );
		}

		hResult = D3DXGetImageInfoFromFile( path.c_str(), &info );
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
		&d3dTexture );
	if (hResult != D3D_OK) {
		string errorMessage = "Failed to load texture:\n" + path;
		throw Exception( errorMessage );
	}

	texture->SetTexture( d3dTexture, info );
}

Font* DirectX::createFont( const string& name, int height, bool isBolded )
{
	// Generate D3D font.
	ID3DXFont *d3dFont;
	HRESULT hr = D3DXCreateFont(
		d3dDevice_,
		height,
		0, 
		isBolded ? FW_BOLD : FW_NORMAL,
		0,
		false,
		DEFAULT_CHARSET,
		OUT_TT_PRECIS,
		CLEARTYPE_NATURAL_QUALITY,
		FF_DECORATIVE,
		name.c_str(),
		&d3dFont );

	if (hr != D3D_OK) {
		throw Exception( "Failed to create font." );
	}

	return new Font( d3dFont );
}

void DirectX::createTexturedQuad( TextureVertex* vertices, float x, float y, int width, int height, D3DCOLOR colour ) {
	// Set universals.
	for (int i = 0; i < 4; i++) {
		vertices[i].z = 1.0f;
		vertices[i].rhw = 0.5f;
		vertices[i].colour = colour;
	}

	// Set corners.
	TextureVertex *topLeft = &vertices[1];
	TextureVertex *bottomLeft = &vertices[0];
	TextureVertex *bottomRight = &vertices[2];
	TextureVertex *topRight = &vertices[3];

	x -= 0.5f;
	y -= 0.5f;
	
	// Top left.
	topLeft->x = x;
	topLeft->y = y;
	topLeft->tu = 0;
	topLeft->tv = 0;

	// Top right.
	topRight->x = x + (float)width;
	topRight->y = y;
	topRight->tu = 1;
	topRight->tv = 0;

	// Bottom right.
	bottomRight->x = x + (float)width;
	bottomRight->y = y + (float)height;
	bottomRight->tu = 1;
	bottomRight->tv = 1;

	// Bottom left.
	bottomLeft->x = x;
	bottomLeft->y = y + (float)height;
	bottomLeft->tu = 0;
	bottomLeft->tv = 1;
}

void DirectX::createColouredQuad( ColourVertex* vertices, float x, float y, int width, int height, D3DCOLOR colour ) {
	// Set universals.
	for (int i = 0; i < 4; i++) {
		vertices[i].z = 1.0f;
		vertices[i].rhw = 1.0f;
		vertices[i].colour = colour;
	}

	// Set corners.
	ColourVertex *topLeft = &vertices[1];
	ColourVertex *bottomLeft = &vertices[0];
	ColourVertex *bottomRight = &vertices[2];
	ColourVertex *topRight = &vertices[3];

	x -= 0.5f;
	y -= 0.5f;
	
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

void DirectX::drawRoundedRect( float x, float y, int width, int height, float radiusTl, float radiusTr, float radiusBr, float radiusBl, D3DCOLOR colour ) {
	// Draw top-left and top.
	createTexturedQuad( texVertices_, x, y, radiusTl, radiusTl, colour );
	drawTexturedQuad( texVertices_, roundedCorner_ );
	createColouredQuad( clrVertices_, x + radiusTl, y, width - radiusTl - radiusTr, radiusTl, colour );
	drawColouredQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Draw top-right and right.
	createTexturedQuad( texVertices_, x + width, y, -radiusTr, radiusTr, colour );
	drawTexturedQuad( texVertices_, roundedCorner_ );
	createColouredQuad( clrVertices_, x + width, y + radiusTr, -radiusTr, height - radiusTr - radiusBr, colour );
	drawColouredQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Draw bottom-right and bottom.
	createTexturedQuad( texVertices_, x + width, y + height, -radiusBr, -radiusBr, colour );
	drawTexturedQuad( texVertices_, roundedCorner_ );
	createColouredQuad( clrVertices_, x + radiusBl, y + height, width - radiusBl - radiusBr, -radiusBr, colour );
	drawColouredQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Draw bottom-left and left.
	createTexturedQuad( texVertices_, x, y + height, radiusBl, -radiusBl, colour );
	drawTexturedQuad( texVertices_, roundedCorner_ );
	createColouredQuad( clrVertices_, x, y + radiusTl, radiusBl, height - radiusTl - radiusBl, colour );
	drawColouredQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Fill in center.
	createColouredQuad( clrVertices_, x + radiusBl, y + radiusTl, width - radiusBl - radiusTr, height - radiusTl - radiusBr, colour );
	drawColouredQuad( clrVertices_, sizeof( clrVertices_ ) );
}

void DirectX::drawRoundedRect( float x, float y, int width, int height, float radius, D3DCOLOR color )
{
	drawRoundedRect( x, y, width, height, radius, radius, radius, radius, color );
}

void DirectX::drawColouredQuad( void *vertices, size_t verticesSize )
{
	// Copy information and unlock.
	void *verticesResult;
	HRESULT hResult = vertexBuffer_->Lock( 0, 0, &verticesResult, 0 );
	if (hResult != D3D_OK) {
		throw Exception( "Failed to lock vertex buffer." );
	}

	memcpy( verticesResult, vertices, verticesSize );
	vertexBuffer_->Unlock();

	// Take colour and alpha from diffuse.
	d3dDevice_->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	d3dDevice_->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

	// Draw quad.
	d3dDevice_->SetFVF( D3D9T_COLOURVERTEX );
	d3dDevice_->SetStreamSource( 0, vertexBuffer_, 0, sizeof( ColourVertex ) );
	d3dDevice_->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
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

	// Begin drawing with sprite.
	return true;
}

void DirectX::endDraw()
{
	/* End scene. */
	d3dDevice_->EndScene();

	/* Present scene. */
	d3dDevice_->Present(NULL, NULL, NULL, NULL);
}

void DirectX::drawTexturedQuad( TextureVertex *texVertices, Texture* texture ) {
	// Add to vertex buffer.
	void *vertices = 0;
	HRESULT hr = vertexBuffer_->Lock( 0, 0, &vertices, 0 );
	memcpy( vertices, texVertices_, sizeof( texVertices_ ) );
	vertexBuffer_->Unlock();

	d3dDevice_->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	d3dDevice_->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

	// Draw quad.
	d3dDevice_->SetFVF( D3D9T_TEXTUREVERTEX );
	d3dDevice_->SetStreamSource( 0, vertexBuffer_, 0, sizeof( TextureVertex ) );
	d3dDevice_->SetTexture( 0, texture->GetTexture() );
	d3dDevice_->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

void DirectX::drawTexture( Texture* texture, float x, float y, float width, float height, const D3DCOLOR& colour)
{
	createTexturedQuad( texVertices_, x, y, width, height );
	drawTexturedQuad( texVertices_, texture );
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
				PostMessage( getHandle(), WM_DESTROY, 0, 0 );
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