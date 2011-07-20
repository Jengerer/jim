#include "directx.h"

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
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
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

	CreateDiffuseBuffer();
	CreateTextureBuffer();

	result = d3dDevice_->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer_ );
	if (FAILED( result )) {
		throw Exception( "Failed to get back buffer surface." );
	}

	// Set projection matrices for 2D.
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	d3dDevice_->SetTransform( D3DTS_WORLD, &matIdentity );
	d3dDevice_->SetTransform( D3DTS_VIEW, &matIdentity );
	SetProjectionSize( GetWidth(), GetHeight() );

	// Set render and stage states.
	d3dDevice_->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	d3dDevice_->SetRenderState( D3DRS_LIGHTING, FALSE );
	
	d3dDevice_->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	// Set alpha blending modes.
	d3dDevice_->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	d3dDevice_->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
	SetBlendState( D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
	d3dDevice_->SetRenderState( D3DRS_ZENABLE, FALSE );

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

void DirectX::SetProjectionSize( float width, float height )
{
	D3DXMATRIX matProjection, matTranslation;
	D3DXMatrixOrthoLH( &matProjection, width, -height, 0.0f, 1.0f );
	D3DXMatrixTranslation( &matTranslation, -width / 2.0f - 0.5f, -height / 2.0f - 0.5f, 0.0f );
	d3dDevice_->SetTransform( D3DTS_PROJECTION, &(matTranslation * matProjection) );
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

void DirectX::SetBlendState( DWORD sourceBlend, DWORD destBlend )
{
	d3dDevice_->SetRenderState( D3DRS_SRCBLEND, sourceBlend );
	d3dDevice_->SetRenderState( D3DRS_DESTBLEND, destBlend );
}

Texture* DirectX::CreateTexture( int width, int height )
{
	// Create empty texture.
	IDirect3DTexture9* emptyTexture = nullptr;
	HRESULT result = d3dDevice_->CreateTexture(
		width, height,
		1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&emptyTexture,
		nullptr );

	if ( FAILED( result ) ) {
		throw Exception( "Failed to create texture." );
	}
	
	// Generate image information.
	D3DXIMAGE_INFO newInformation;
	ZeroMemory( &newInformation, sizeof( D3DXIMAGE_INFO ) );
	newInformation.Width = width;
	newInformation.Height = height;
	newInformation.Format = D3DFMT_A8R8G8B8;

	// Generate texture object and initialize.
	Texture *newTexture = new Texture();
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
		D3DCLEAR_TARGET,
		D3DCOLOR_ARGB( 0, 0, 0, 0 ),
		1.0f, 0 );
	targetSurface->Release();
}

void DirectX::ResetProjectionSize()
{
	SetProjectionSize( GetWidth(), GetHeight() );
}

void DirectX::ResetRenderTarget( void )
{
	// Set to back buffer.
	d3dDevice_->SetRenderTarget( 0, backBuffer_ );
}

Texture* DirectX::GetTexture( const string& filename )
{
	string defaultUrl = "http://www.jengerer.com/itemManager/imgFiles/" + filename + ".png";
	return GetTexture( filename, defaultUrl );
}

Texture* DirectX::GetTexture( const string& filename, const string& url )
{
	// Check if the texture exists already.
	TextureMap::iterator i = textures_->find( filename );
	if (i != textures_->end()) {
		return i->second;
	}

	Texture* texture = new Texture( filename, url );
	try {
		LoadTexture( texture );
	}
	catch (Exception&) {
		// Try to download the texture.
		string path = "imgFiles/" + filename + ".png";
		if (!download( url, path )) {
			string errorMessage = "Failed to load texture:\n" + path;
			throw Exception( errorMessage );
		}

		LoadTexture( texture );
	}

	// Insert and return.
	(*textures_)[filename] = texture;
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

	string filename = texture->GetFilename();
	string path = "imgFiles/" + filename + ".png";
	HRESULT result = D3DXGetImageInfoFromFile( path.c_str(), &info );
	if ( FAILED( result ) ) {
		throw Exception( "Failed to load image information:\n" + filename );
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
		throw Exception( "Failed to load texture:\n" + filename );
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
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		FF_DONTCARE,
		name.c_str(),
		&d3dFont );

	if ( FAILED( result ) ) {
		throw Exception( "Failed to create font." );
	}

	return new Font( d3dFont );
}

void DirectX::CreateDiffuseBuffer( void )
{
	// Create vertex buffer.
	HRESULT result = d3dDevice_->CreateVertexBuffer(
		4 * sizeof(DiffuseVertex),
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		D3D9T_DIFFUSEVERTEX,
		D3DPOOL_DEFAULT,
		&diffuseBuffer_,
		NULL );
	if ( FAILED( result ) ) {
		throw Exception( "Failed to create vertex buffer." );
	}

	// Lock diffuse vertex buffer.
	DiffuseVertex *diffuseVertices = nullptr;
	diffuseBuffer_->Lock( 0, 4 * sizeof( DiffuseVertex ), (void**)&diffuseVertices, 0 );

	// Set universals.
	diffuseVertices[0].z		= diffuseVertices[1].z
								= diffuseVertices[2].z
								= diffuseVertices[3].z
								= 1.0f;

	diffuseVertices[0].colour	= diffuseVertices[1].colour
								= diffuseVertices[2].colour
								= diffuseVertices[3].colour
								= 0xffffffff;
	
	// Top left and bottom left.
	diffuseVertices[0].x = diffuseVertices[1].x = 0.0f;

	// Top left and top right.
	diffuseVertices[0].y = diffuseVertices[2].y = 0.0f;

	// Bottom left and bottom right.
	diffuseVertices[1].y = diffuseVertices[3].y = 1.0f;

	// Top right and bottom right.
	diffuseVertices[2].x = diffuseVertices[3].x = 1.0f;

	// Unlock diffuse vertex buffer.
	diffuseBuffer_->Unlock();
}

void DirectX::CreateTextureBuffer( void )
{
	// Create vertex buffer.
	HRESULT result = d3dDevice_->CreateVertexBuffer(
		4 * sizeof(TextureVertex),
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		D3D9T_TEXTUREVERTEX,
		D3DPOOL_DEFAULT,
		&textureBuffer_,
		NULL );
	if ( FAILED( result ) ) {
		throw Exception( "Failed to create texture vertex buffer." );
	}

	// Lock diffuse vertex buffer.
	TextureVertex *textureVertices = nullptr;
	textureBuffer_->Lock( 0, 4 * sizeof( DiffuseVertex ), (void**)&textureVertices, 0 );

	// Set universals.
	textureVertices[0].z		= textureVertices[1].z
								= textureVertices[2].z
								= textureVertices[3].z
								= 1.0f;

	textureVertices[0].colour	= textureVertices[1].colour
								= textureVertices[2].colour
								= textureVertices[3].colour
								= 0xffffffff;
	
	// Top left and bottom left.
	textureVertices[0].x = textureVertices[1].x = 0.0f;
	textureVertices[0].tu = textureVertices[1].tu = 0.0f;

	// Top left and top right.
	textureVertices[0].y = textureVertices[2].y = 0.0f;
	textureVertices[0].tv = textureVertices[2].tv = 0.0f;

	// Bottom left and bottom right.
	textureVertices[1].y = textureVertices[3].y = 1.0f;
	textureVertices[1].tv = textureVertices[3].tv = 1.0f;

	// Top right and bottom right.
	textureVertices[2].x = textureVertices[3].x = 1.0f;
	textureVertices[2].tu = textureVertices[3].tu = 1.0f;

	// Unlock texture vertex buffer.
	textureBuffer_->Unlock();
}

void DirectX::DrawRoundedRect( float x, float y, int width, int height, float radiusTl, float radiusTr, float radiusBr, float radiusBl, D3DCOLOR colour )
{
	// Draw top-left and top.
	DrawTexture( roundedCorner_, x, y, radiusTl, radiusTl, colour );
	DrawColouredQuad( x + radiusTl, y, width - radiusTl - radiusTr, radiusTl, colour );

	// Draw top-right and right.
	DrawTexture( roundedCorner_, x + width, y, -radiusTr, radiusTr, colour );
	DrawColouredQuad( x + width, y + radiusTr, -radiusTr, height - radiusTr - radiusBr, colour );

	// Draw bottom-right and bottom.
	DrawTexture( roundedCorner_, x + width, y + height, -radiusBr, -radiusBr, colour );
	DrawColouredQuad( x + radiusBl, y + height, width - radiusBl - radiusBr, -radiusBr, colour );

	// Draw bottom-left and left.
	DrawTexture( roundedCorner_, x, y + height, radiusBl, -radiusBl, colour );
	DrawColouredQuad( x, y + radiusTl, radiusBl, height - radiusTl - radiusBl, colour );

	// Fill in center.
	DrawColouredQuad( x + radiusBl, y + radiusTl, width - radiusBl - radiusTr, height - radiusTl - radiusBr, colour );
}

void DirectX::DrawRoundedRect( float x, float y, int width, int height, float radius, D3DCOLOR color )
{
	DrawRoundedRect( x, y, width, height, radius, radius, radius, radius, color );
}

void DirectX::DrawColouredQuad( float x, float y, int width, int height, D3DCOLOR colour )
{	
	// Change diffuse colour.
	// TODO: Need better way of changing colour. This is too expensive.
	DiffuseVertex *diffuseVertices = nullptr;
	diffuseBuffer_->Lock( 0, 4 * sizeof( DiffuseVertex ), (void**)&diffuseVertices, 0 );
	diffuseVertices[0].colour	= diffuseVertices[1].colour
								= diffuseVertices[2].colour
								= diffuseVertices[3].colour
								= colour;
	diffuseBuffer_->Unlock();

	// Take colour and alpha from diffuse.
	d3dDevice_->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	d3dDevice_->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

	// Move and scale.
	D3DXMATRIX matScale, matTrans;
	D3DXMatrixScaling( &matScale, width, height, 1.0f );
	D3DXMatrixTranslation( &matTrans, x, y, 0.0f );
	d3dDevice_->SetTransform( D3DTS_WORLD, &(matScale * matTrans) );

	// Draw quad.
	d3dDevice_->SetFVF( D3D9T_DIFFUSEVERTEX );
	d3dDevice_->SetStreamSource( 0, diffuseBuffer_, 0, sizeof( DiffuseVertex ) );
	d3dDevice_->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

void DirectX::DrawTexture( Texture* texture, float x, float y, int width, int height, D3DCOLOR colour )
{
	// Change diffuse on texture.
	// TODO: Need better way of changing colour. This is too expensive.
	TextureVertex *textureVertices = nullptr;
	textureBuffer_->Lock( 0, 4 * sizeof( TextureVertex ), (void**)&textureVertices, 0 );
	textureVertices[0].colour	= textureVertices[1].colour
								= textureVertices[2].colour
								= textureVertices[3].colour
								= colour;
	textureBuffer_->Unlock();

	// Take colour and alpha from modulation.
	d3dDevice_->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	d3dDevice_->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

	// Set drawing parameters.
	d3dDevice_->SetFVF( D3D9T_TEXTUREVERTEX );
	d3dDevice_->SetTexture( 0, texture->GetTexture() );

	// Move and scale.
	D3DXMATRIX matScale, matTrans;
	D3DXMatrixScaling( &matScale, width, height, 1.0f );
	D3DXMatrixTranslation( &matTrans, x, y, 0.0f );
	d3dDevice_->SetTransform( D3DTS_WORLD, &(matScale * matTrans) );

	// Draw.
	d3dDevice_->SetStreamSource( 0, textureBuffer_, 0, sizeof( TextureVertex ) );
	d3dDevice_->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

void DirectX::SetWorldTransform( D3DXMATRIX *matrix )
{
	// Copy it over.
	matWorld_ = *matrix;
}

void DirectX::GetWorldTransform( D3DXMATRIX *matrix )
{
	*matrix = matWorld_;
}

bool DirectX::BeginDraw( void )
{
	// Ensure we have all interfaces functional.
	if ( !CheckDevice() ) {
		return false;
	}

#if defined( _DEBUG )
	if (GetAsyncKeyState( VK_UP ) & 0x8000) {
		d3dDevice_->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	}
	else if (GetAsyncKeyState( VK_DOWN ) & 0x8000) {
		d3dDevice_->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	}
#endif

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
	D3DXMATRIX d3dTrans;
	d3dDevice_->GetTransform( D3DTS_PROJECTION, &d3dTrans );
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