#include "DirectX.h"

const D3DCOLOR WHITE_COLOUR = D3DCOLOR_ARGB( 255, 255, 255, 255 );
const D3DCOLOR BACKGROUND_COLOUR = D3DCOLOR_ARGB( 255, 43, 39, 37 );

DirectX::DirectX( HINSTANCE hInstance,
	const char *title,
	int width, int height ) : Window( hInstance, title, width, height )
{
	textures_ = nullptr;
	roundedCorner_ = nullptr;
}

DirectX::~DirectX()
{
	//DirectX has been destroyed.
	CloseInterfaces();
}

void DirectX::LoadInterfaces( void )
{
	d3d_ = Direct3DCreate9( D3D_SDK_VERSION );
	if (d3d_ == nullptr) {
		throw Exception( "Failed to create Direct3D." );
	}

	D3DDISPLAYMODE d3dMode;
	d3d_->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3dMode );

	ZeroMemory( &params_, sizeof( D3DPRESENT_PARAMETERS ) );
	params_.Windowed			= TRUE;
	params_.SwapEffect			= D3DSWAPEFFECT_DISCARD;
	params_.BackBufferFormat	= d3dMode.Format;

	HRESULT result = d3d_->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		getHandle(),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&params_,
		&d3dDevice_);
	if ( FAILED( result ) ) {
		throw Exception( "Failed to create Direct3D device!" );
	}

	// Load font as resource.
	if (AddFontResourceEx("ttfFiles/tf2Build.ttf", FR_PRIVATE, 0) == 0) {
		if (!download("www.jengerer.com/itemManager/ttfFiles/tf2Build.ttf", "ttfFiles/tf2Build.ttf") || !AddFontResource("ttfFiles/tf2Build.ttf")) {
			throw Exception( "Failed to load font." );
		}
	}

	// Load font as resource.
	if (AddFontResourceEx("ttfFiles/tf2Secondary.ttf", FR_PRIVATE, 0) == 0) {
		if (!download("www.jengerer.com/itemManager/ttfFiles/tf2Secondary.ttf", "ttfFiles/tf2Secondary.ttf") || !AddFontResource("ttfFiles/tf2Secondary.ttf")) {
			throw Exception( "Failed to load font." );
		}
	}

	// Create vertex buffer.
	result = d3dDevice_->CreateVertexBuffer(
		4 * sizeof(TextureVertex),
		D3DUSAGE_WRITEONLY,
		D3D9T_TEXTUREVERTEX,
		D3DPOOL_DEFAULT,
		&vertexBuffer_,
		NULL );
	if ( FAILED( result ) ) {
		throw Exception( "Failed to create vertex buffer." );
	}

	result = d3dDevice_->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer_ );
	if ( FAILED( result ) ) {
		throw Exception( "Failed to get back buffer surface." );
	}

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

	// Create empty hashtable.
	textures_ = new TextureMap();

	// Load round corner.
	roundedCorner_ = GetTexture( "manager/rounded_corner" );
}

void DirectX::CloseInterfaces( void )
{
	// Delete map of vectors.
	if (textures_ != nullptr) {
		FreeTextures();
		delete textures_;
		textures_ = nullptr;
	}

	// Remove font resource.
	RemoveFontResourceEx( "ttfFiles/tf2Build.ttf", FR_PRIVATE, 0 );
	RemoveFontResourceEx( "ttfFiles/tf2Secondary.ttf", FR_PRIVATE, 0 );

	// Free backbuffer.
	if (backBuffer_ != nullptr) {
		backBuffer_->Release();
		backBuffer_ = nullptr;
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

void DirectX::FreeTextures( void )
{
	// Delete all texture objects.
	TextureMap::iterator i;
	while ( (i = textures_->begin()) != textures_->end() ) {
		Texture *texture = i->second;
		textures_->erase( i );
		delete texture;
	}
}

void DirectX::LoadTextures( void )
{
	// Reload any existing unloaded textures.
	TextureMap::iterator i;
	for (i = textures_->begin(); i != textures_->end(); i++) {
		Texture *texture = i->second;
		if ( !texture->IsLoaded() ) {
			LoadTexture( texture );
		}
	}
}

void DirectX::ReleaseTextures( void )
{
	// Get textures and release them.
	TextureMap::iterator i;
	for (i = textures_->begin(); i != textures_->end(); i++) {
		Texture *texture = i->second;
		texture->ReleaseTexture();
	}
}

Texture* DirectX::CreateTexture( const string& name, int width, int height )
{
	// Create empty texture.
	IDirect3DTexture9* emptyTexture = nullptr;
	d3dDevice_->CreateTexture(
		width, height,
		1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&emptyTexture,
		nullptr );
	
	// Generate image information.
	D3DXIMAGE_INFO newInformation;
	ZeroMemory( &newInformation, sizeof( D3DXIMAGE_INFO ) );
	newInformation.Width = width;
	newInformation.Height = height;
	newInformation.Format = D3DFMT_A8R8G8B8;

	// Generate texture object and initialize.
	Texture *newTexture = new Texture( name );
	newTexture->SetTexture( emptyTexture, newInformation );
	return newTexture;
}

void DirectX::SetRenderTarget( Texture *texture )
{
	IDirect3DTexture9* targetTexture = texture->GetTexture();
	IDirect3DSurface9* targetSurface = nullptr;

	// Get the surface to render to.
	HRESULT result = targetTexture->GetSurfaceLevel( 0, &targetSurface );
	if ( FAILED( result ) ) {
		throw Exception( "Failed to get texture surface for rendering." );
	}

	d3dDevice_->SetRenderTarget( 0, targetSurface );
	d3dDevice_->Clear(
		0,
		nullptr,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_ARGB( 0, 0, 0, 0 ),
		1.0f, 0 );
	targetSurface->Release();
}

void DirectX::ResetRenderTarget( void )
{
	// Set to back buffer.
	d3dDevice_->SetRenderTarget( 0, backBuffer_ );
}

Texture* DirectX::GetTexture( const string& filename )
{
	// Check if the texture exists already.
	TextureMap::iterator i = textures_->find( filename );
	if (i != textures_->end()) {
		return i->second;
	}

	// Create and load.
	Texture* texture = new Texture( filename );
	LoadTexture( texture );

	// Insert and return.
	TexturePair texturePair( filename, texture );
	textures_->insert( texturePair );
	return texture;
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
	HRESULT result = D3DXGetImageInfoFromFile( path.c_str(), &info );
	if ( FAILED( result ) ) {
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

		result = D3DXGetImageInfoFromFile( path.c_str(), &info );
		if ( FAILED( result ) ) {
			string errorMessage = "Failed to load texture:\n" + path;
			throw Exception( errorMessage );
		}
	}

	// Load the texture.
	result = D3DXCreateTextureFromFileEx(
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
	if ( FAILED( result ) ) {
		string errorMessage = "Failed to load texture:\n" + path;
		throw Exception( errorMessage );
	}

	// Finally, load the texture in.
	texture->SetTexture( d3dTexture, info );
}

Font* DirectX::CreateFont( const string& name, int height, bool isBolded )
{
	// Generate D3D font.
	ID3DXFont *d3dFont = nullptr;
	HRESULT result = D3DXCreateFont(
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

	if ( FAILED( result ) ) {
		throw Exception( "Failed to create font." );
	}

	return new Font( d3dFont );
}

void DirectX::CreateTexturedQuad( TextureVertex* vertices, float x, float y, int width, int height, D3DCOLOR colour )
{
	// Set universals.
	for (int i = 0; i < 4; i++) {
		vertices[i].z = 1.0f;
		vertices[i].rhw = 0.5f;
		vertices[i].colour = colour;
	}

	x -= 0.5f;
	y -= 0.5f;
	
	// Top left.
	vertices[1].x = x;
	vertices[1].y = y;
	vertices[1].tu = 0;
	vertices[1].tv = 0;

	// Top right.
	vertices[3].x = x + (float)width;
	vertices[3].y = y;
	vertices[3].tu = 1;
	vertices[3].tv = 0;

	// Bottom right.
	vertices[2].x = x + (float)width;
	vertices[2].y = y + (float)height;
	vertices[2].tu = 1;
	vertices[2].tv = 1;

	// Bottom left.
	vertices[0].x = x;
	vertices[0].y = y + (float)height;
	vertices[0].tu = 0;
	vertices[0].tv = 1;
}

void DirectX::CreateColouredQuad( ColourVertex* vertices, float x, float y, int width, int height, D3DCOLOR colour )
{
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
	vertices[1].x = x;
	vertices[1].y = y;

	// Top right.
	vertices[3].x = x + (float)width;
	vertices[3].y = y;
	
	// Bottom right.
	vertices[2].x = x + (float)width;
	vertices[2].y = y + (float)height;

	// Bottom left.
	vertices[0].x = x;
	vertices[0].y = y + (float)height;
}

void DirectX::DrawRoundedRect( float x, float y, int width, int height, float radiusTl, float radiusTr, float radiusBr, float radiusBl, D3DCOLOR colour )
{
	// Draw top-left and top.
	CreateTexturedQuad( texVertices_, x, y, radiusTl, radiusTl, colour );
	DrawTexturedQuad( texVertices_, roundedCorner_ );
	CreateColouredQuad( clrVertices_, x + radiusTl, y, width - radiusTl - radiusTr, radiusTl, colour );
	DrawColouredQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Draw top-right and right.
	CreateTexturedQuad( texVertices_, x + width, y, -radiusTr, radiusTr, colour );
	DrawTexturedQuad( texVertices_, roundedCorner_ );
	CreateColouredQuad( clrVertices_, x + width, y + radiusTr, -radiusTr, height - radiusTr - radiusBr, colour );
	DrawColouredQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Draw bottom-right and bottom.
	CreateTexturedQuad( texVertices_, x + width, y + height, -radiusBr, -radiusBr, colour );
	DrawTexturedQuad( texVertices_, roundedCorner_ );
	CreateColouredQuad( clrVertices_, x + radiusBl, y + height, width - radiusBl - radiusBr, -radiusBr, colour );
	DrawColouredQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Draw bottom-left and left.
	CreateTexturedQuad( texVertices_, x, y + height, radiusBl, -radiusBl, colour );
	DrawTexturedQuad( texVertices_, roundedCorner_ );
	CreateColouredQuad( clrVertices_, x, y + radiusTl, radiusBl, height - radiusTl - radiusBl, colour );
	DrawColouredQuad( clrVertices_, sizeof( clrVertices_ ) );

	// Fill in center.
	CreateColouredQuad( clrVertices_, x + radiusBl, y + radiusTl, width - radiusBl - radiusTr, height - radiusTl - radiusBr, colour );
	DrawColouredQuad( clrVertices_, sizeof( clrVertices_ ) );
}

void DirectX::DrawRoundedRect( float x, float y, int width, int height, float radius, D3DCOLOR color )
{
	DrawRoundedRect( x, y, width, height, radius, radius, radius, radius, color );
}

void DirectX::DrawColouredQuad( void *vertices, size_t verticesSize )
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

void DirectX::DrawTexturedQuad( TextureVertex *texVertices, Texture* texture )
{
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

void DirectX::DrawTexture( Texture* texture, float x, float y, float width, float height, const D3DCOLOR& colour)
{
	CreateTexturedQuad( texVertices_, x, y, width, height, colour );
	DrawTexturedQuad( texVertices_, texture );
}

bool DirectX::BeginDraw( void )
{
	// Ensure we have all interfaces functional.
	if ( !CheckDevice() ) {
		return false;
	}

	// Clear background.
	d3dDevice_->Clear( 0, NULL, D3DCLEAR_TARGET, BACKGROUND_COLOUR, 1.0f, 0 );
	HRESULT result = d3dDevice_->BeginScene();
	if ( FAILED( result ) ) {
		return false;
	}

	// Begin drawing with sprite.
	return true;
}

void DirectX::EndDraw( void )
{
	// End scene and present.
	d3dDevice_->EndScene();
	d3dDevice_->Present(NULL, NULL, NULL, NULL);
}

bool DirectX::CheckDevice( void )
{
	HRESULT result = d3dDevice_->TestCooperativeLevel();
	switch (result) {
	case D3DERR_DEVICELOST:
		return false;

	case D3DERR_DEVICENOTRESET:
		{
			// Release all textures.
			ReleaseTextures();

			// Refresh the sprite.
			HRESULT result = d3dDevice_->Reset( &params_ );
			if ( FAILED( result ) ) {
				throw Exception( "Failed to reset Direct3D device." );
				PostMessage( getHandle(), WM_DESTROY, 0, 0 );
			}

			// Reload all textures.
			LoadTextures();
		}

		return false;
		break;

	default:
		return true;
	}
}