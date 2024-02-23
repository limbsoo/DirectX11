////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "rendertextureclass.h"
#include "SharedDefines.h"

RenderTextureClass::RenderTextureClass()
{
	m_renderTargetTexture = 0;
	m_renderTargetView = 0;
	m_shaderResourceView = 0;
	m_renderTargetTextureDSV = 0;
	m_depthStencilView = 0;
}


RenderTextureClass::RenderTextureClass(const RenderTextureClass& other)
{
}


RenderTextureClass::~RenderTextureClass()
{
}


bool RenderTextureClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;


	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);

	if (FAILED(result))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC textureDesc_; /// Setup the render target texture description.
	ZeroMemory(&textureDesc_, sizeof(textureDesc_)); /// Initialize the render target texture description.
	textureDesc_.Width = textureWidth;
	textureDesc_.Height = textureHeight;
	textureDesc_.MipLevels = 0;
	textureDesc_.ArraySize = 1;
	textureDesc_.Format = DXGI_FORMAT_R24G8_TYPELESS;
	textureDesc_.SampleDesc.Count = 1;
	textureDesc_.SampleDesc.Quality = 0;
	textureDesc_.Usage = D3D11_USAGE_DEFAULT;
	textureDesc_.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc_.CPUAccessFlags = 0;
	textureDesc_.MiscFlags = 0;

	result = device->CreateTexture2D(&textureDesc_, NULL, &m_renderTargetTextureDSV);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Texture2D'", L"Error", MB_OK | MB_ICONERROR); }

	/// Create the depth stencil view desc
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	result = device->CreateDepthStencilView(m_renderTargetTextureDSV, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Depth Stencil View'", L"Error", MB_OK | MB_ICONERROR); }

	return true;
}

bool RenderTextureClass::InitializeBackBufferWithMipMap(ID3D11Device* device, int Width, int Height)
{
	HRESULT result;

	D3D11_TEXTURE2D_DESC textureDesc; /// Setup the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc)); /// Initialize the render target texture description.
	textureDesc.Width = Width;
	textureDesc.Height = Height;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // DXGI_FORMAT_R16G16B16A16_FLOAT
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	/// Create the render target texture.
	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Texture2D'", L"Error", MB_OK | MB_ICONERROR); }

	/// Setup the description of the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // DXGI_FORMAT_R16G16B16A16_FLOAT
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	/// Create the render target view.
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Render Target View'", L"Error", MB_OK | MB_ICONERROR); }

	/// Setup the description of the shader resource view.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // DXGI_FORMAT_R16G16B16A16_FLOAT
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = -1; // -1 means automatically setting the max mipmap level
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	/// Create the shader resource view.
	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Shader Resource View'", L"Error", MB_OK | MB_ICONERROR); }

	//------------------------------------------------------------------------------
	// Also Create depth stencil buffer
	// In order to a situation that back buffer size bigger than window size
	//------------------------------------------------------------------------------
	D3D11_TEXTURE2D_DESC textureDesc_; /// Setup the render target texture description.
	ZeroMemory(&textureDesc_, sizeof(textureDesc_)); /// Initialize the render target texture description.
	textureDesc_.Width = Width;
	textureDesc_.Height = Height;
	textureDesc_.MipLevels = 1;
	textureDesc_.ArraySize = 1;
	textureDesc_.Format = DXGI_FORMAT_R24G8_TYPELESS;
	textureDesc_.SampleDesc.Count = 1;
	textureDesc_.SampleDesc.Quality = 0;
	textureDesc_.Usage = D3D11_USAGE_DEFAULT;
	textureDesc_.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc_.CPUAccessFlags = 0;
	textureDesc_.MiscFlags = 0;
	/// Create the render target texture.
	result = device->CreateTexture2D(&textureDesc_, NULL, &m_renderTargetTexture);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Texture2D'", L"Error", MB_OK | MB_ICONERROR); }

	/// Create the depth stencil view desc
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	result = device->CreateDepthStencilView(m_renderTargetTexture, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Depth Stencil View'", L"Error", MB_OK | MB_ICONERROR); }

	return true;
}

bool RenderTextureClass::SetInitialize(ID3D11Device* pdevice, int textureWidth, int textureHeight) {

	//renertarget shaderResourceView depthStencilView
	HRESULT result;

	// Initialize the render target texture description.
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = pdevice->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = pdevice->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view.
	result = pdevice->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);

	if (FAILED(result))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC textureDesc_; /// Setup the render target texture description.
	ZeroMemory(&textureDesc_, sizeof(textureDesc_)); /// Initialize the render target texture description.
	textureDesc_.Width = textureWidth;
	textureDesc_.Height = textureHeight;
	textureDesc_.MipLevels = 0;
	textureDesc_.ArraySize = 1;
	textureDesc_.Format = DXGI_FORMAT_R24G8_TYPELESS;
	textureDesc_.SampleDesc.Count = 1;
	textureDesc_.SampleDesc.Quality = 0;
	textureDesc_.Usage = D3D11_USAGE_DEFAULT;
	textureDesc_.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc_.CPUAccessFlags = 0;
	textureDesc_.MiscFlags = 0;

	result = pdevice->CreateTexture2D(&textureDesc_, NULL, &m_renderTargetTextureDSV);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Texture2D'", L"Error", MB_OK | MB_ICONERROR); }

	/// Create the depth stencil view desc
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	result = pdevice->CreateDepthStencilView(m_renderTargetTextureDSV, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Depth Stencil View'", L"Error", MB_OK | MB_ICONERROR); }

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xff;
	dsDesc.StencilWriteMask = 0xff;

	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	pdevice->CreateDepthStencilState(&dsDesc, &m_WriteDepthStencilState);

	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_DECR_SAT;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR_SAT;

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_DECR_SAT;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR_SAT;

	pdevice->CreateDepthStencilState(&dsDesc, &m_DecrementdepthStencilState);

	D3D11_BLEND_DESC bsDesc;
	ZeroMemory(&bsDesc, sizeof(D3D11_BLEND_DESC));
	pdevice->CreateBlendState(&bsDesc, &m_pBlendState);

	D3D11_RASTERIZER_DESC rsDesc;
	rsDesc.AntialiasedLineEnable = false;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.ScissorEnable = false;
	rsDesc.SlopeScaledDepthBias = 0.0f;

	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.MultisampleEnable = true;

	pdevice->CreateRasterizerState(&rsDesc, &m_pRasterState);

	D3D11_QUERY_DESC qDesc;
	qDesc.Query = D3D11_QUERY_OCCLUSION;
	qDesc.MiscFlags = 0;
	pdevice->CreateQuery(&qDesc, &m_pQuery);

	return true;
}

bool RenderTextureClass::SetMSAAInitialize(ID3D11Device* pdevice, int textureWidth, int textureHeight) {

	HRESULT result;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = MSAA_LEVEL;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	UINT msaa_quality;
	result = pdevice->CheckMultisampleQualityLevels(textureDesc.Format, MSAA_LEVEL, &msaa_quality);
	msaa_quality--;
	textureDesc.SampleDesc.Quality = msaa_quality;

	result = pdevice->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

	result = pdevice->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	//shaderResourceViewDesc.Texture2D.MipLevels = 1;

	result = pdevice->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);

	if (FAILED(result))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC textureDesc_;
	ZeroMemory(&textureDesc_, sizeof(textureDesc_));
	textureDesc_.Width = textureWidth;
	textureDesc_.Height = textureHeight;
	textureDesc_.MipLevels = 1;
	textureDesc_.ArraySize = 1;
	textureDesc_.Format = DXGI_FORMAT_R24G8_TYPELESS;
	textureDesc_.SampleDesc.Count = MSAA_LEVEL;
	textureDesc_.Usage = D3D11_USAGE_DEFAULT;
	textureDesc_.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc_.CPUAccessFlags = 0;
	textureDesc_.MiscFlags = 0;
	textureDesc_.SampleDesc.Quality = msaa_quality;

	result = pdevice->CreateTexture2D(&textureDesc_, NULL, &m_renderTargetTextureDSV);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Texture2D'", L"Error", MB_OK | MB_ICONERROR); }

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	result = pdevice->CreateDepthStencilView(m_renderTargetTextureDSV, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Depth Stencil View'", L"Error", MB_OK | MB_ICONERROR); }

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.DepthEnable = 0;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.StencilEnable = 0;
	dsDesc.StencilReadMask = 0xff;
	dsDesc.StencilWriteMask = 0xff;

	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	pdevice->CreateDepthStencilState(&dsDesc, &m_WriteDepthStencilState);

	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_DECR_SAT;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR_SAT;

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_DECR_SAT;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR_SAT;

	pdevice->CreateDepthStencilState(&dsDesc, &m_DecrementdepthStencilState);

	D3D11_BLEND_DESC bsDesc;
	ZeroMemory(&bsDesc, sizeof(D3D11_BLEND_DESC));
	pdevice->CreateBlendState(&bsDesc, &m_pBlendState);

	D3D11_RASTERIZER_DESC rsDesc;/*
	rsDesc.AntialiasedLineEnable = false;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.ScissorEnable = false;
	rsDesc.SlopeScaledDepthBias = 0.0f;

	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.MultisampleEnable = false;*/

	ZeroMemory(&rsDesc, sizeof(rsDesc));

	rsDesc.AntialiasedLineEnable = true;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.DepthBiasClamp = 0.f;
	rsDesc.DepthClipEnable = true;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.MultisampleEnable = true;
	rsDesc.ScissorEnable = false;
	rsDesc.SlopeScaledDepthBias = 0.0f;

	pdevice->CreateRasterizerState(&rsDesc, &m_pRasterState);

	D3D11_QUERY_DESC qDesc;
	ZeroMemory(&qDesc, sizeof(qDesc));
	qDesc.Query = D3D11_QUERY_OCCLUSION;
	qDesc.MiscFlags = 0;
	pdevice->CreateQuery(&qDesc, &m_pQuery);

	return true;
}

void RenderTextureClass::Shutdown()
{

	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = 0;
	}


	if (m_renderTargetTextureDSV)
	{
		m_renderTargetTextureDSV->Release();
		m_renderTargetTextureDSV = 0;
	}

	//if (m_UsedepthStencilState)
	//{
	//	m_UsedepthStencilState->Release();
	//	m_UsedepthStencilState = 0;
	//}

	//if (m_DecrementdepthStencilState)
	//{
	//	m_DecrementdepthStencilState->Release();
	//	m_DecrementdepthStencilState = 0;
	//}

	//if (m_pRasterState)
	//{
	//	m_pRasterState->Release();
	//	m_pRasterState = 0;
	//}
	//
	//if (m_pBlendState)
	//{
	//	m_pBlendState->Release();
	//	m_pBlendState = 0;
	//}
	//
	//if (m_pQuery)
	//{
	//	m_pQuery->Release();
	//	m_pQuery = 0;
	//}
}


void RenderTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);
}

void RenderTextureClass::CreateMipmap(ID3D11DeviceContext* deviceContext)
{
	deviceContext->GenerateMips(m_shaderResourceView);
}

void RenderTextureClass::Clear(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView,
	float red, float green, float blue, float alpha)
{
	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* RenderTextureClass::GetShaderResourceView()
{
	return m_shaderResourceView;
}

ID3D11RenderTargetView* RenderTextureClass::GetRenderTargetView()
{
	return m_renderTargetView;
}

ID3D11DepthStencilView* RenderTextureClass::GetDepthStencilView()
{
	return m_depthStencilView;
}

ID3D11DepthStencilState* RenderTextureClass::GetWriteDepthStencilState()
{
	return m_WriteDepthStencilState;
}

ID3D11DepthStencilState* RenderTextureClass::GetDecrementDepthStencilState()
{
	return m_DecrementdepthStencilState;
}

ID3D11RasterizerState* RenderTextureClass::GetRasterizerState()
{
	return m_pRasterState;
}

ID3D11BlendState* RenderTextureClass::GetBlendState()
{
	return m_pBlendState;
}

ID3D11Query* RenderTextureClass::GetQuery()
{
	return m_pQuery;
}