////////////////////////////////////////////////////////////////////////////////
// Filename: RenderTexture.cpp
////////////////////////////////////////////////////////////////////////////////
#include "RenderTexture.h"
#include "define.h"
#include <d3dx11effect.h>



RenderTexture::RenderTexture()
{
	//m_renderTargetTexture_MSAA = 0;
	//depthStencilTexture_MSAA = 0;
	//m_renderTargetView_MSAA = 0;
	//m_shaderResourceView_MSAA = 0;
	//m_renderTargetTextureDSV = 0;
	//m_depthStencilView_MSAA = 0;
}


RenderTexture::RenderTexture(const RenderTexture& other)
{
}


RenderTexture::~RenderTexture()
{
}


bool RenderTexture::Initialize(ID3D11Device* device, int textureWidth, int textureHeight)
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

	//------------------------------------------------------------------------------
   // Also Create depth stencil buffer
   // In order to a situation that back buffer size bigger than window size
   //------------------------------------------------------------------------------
   /// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476253(v=vs.85).aspx
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
	/// Create the render target texture.
	/// https://msdn.microsoft.com/ko-kr/library/windows/desktop/ff476521(v=vs.85).aspx
	result = device->CreateTexture2D(&textureDesc_, NULL, &m_depthStencilTexture);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Texture2D'", L"Error", MB_OK | MB_ICONERROR); }

	/// Create the depth stencil view desc
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	result = device->CreateDepthStencilView(m_depthStencilTexture, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Depth Stencil View'", L"Error", MB_OK | MB_ICONERROR); }

	return true;
}

bool RenderTexture::Initialize_DepthBuffer_DepthPeeling(ID3D11Device* pdevice, int textureWidth, int textureHeight)
{
	HRESULT result;

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = textureWidth;
	depthStencilDesc.Height = textureHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	result = pdevice->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilTexture);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'DepthBufferDepthStencil'", L"Error", MB_OK | MB_ICONERROR); }

	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	dsv_desc.Flags = 0;
	dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Texture2D.MipSlice = 0;

	result = pdevice->CreateDepthStencilView(m_depthStencilTexture, &dsv_desc, &m_depthStencilView);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'DepthBufferDepthStencilView'", L"Error", MB_OK | MB_ICONERROR); }

	D3D11_SHADER_RESOURCE_VIEW_DESC sr_desc;
	sr_desc.Format = DXGI_FORMAT_R32_FLOAT;
	sr_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sr_desc.Texture2D.MostDetailedMip = 0;
	sr_desc.Texture2D.MipLevels = 1;

	result = pdevice->CreateShaderResourceView(m_depthStencilTexture, &sr_desc, &m_shaderResourceView);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'DepthBufferShaderResourceView'", L"Error", MB_OK | MB_ICONERROR); }

	return true;
}

bool RenderTexture::InitializeBackBufferWithMipMap_Array(ID3D11Device* device, int Width, int Height)
{
	HRESULT result;

	D3D11_TEXTURE2D_DESC textureDesc; /// Setup the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc)); /// Initialize the render target texture description.
	textureDesc.Width = Width;
	textureDesc.Height = Height;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = N_LAYER;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // DXGI_FORMAT_R16G16B16A16_FLOAT
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Texture2D'", L"Error", MB_OK | MB_ICONERROR); }

	result = device->CreateRenderTargetView(m_renderTargetTexture, NULL, &m_renderTargetView);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'LayerRenderTargetView'", L"Error", MB_OK | MB_ICONERROR); }

	result = device->CreateShaderResourceView(m_renderTargetTexture, NULL, &m_shaderResourceView);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'LayerShaderResourceView'", L"Error", MB_OK | MB_ICONERROR); }

	D3D11_RENDER_TARGET_VIEW_DESC layerViewDesc;
	m_renderTargetView->GetDesc(&layerViewDesc);
	layerViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	layerViewDesc.Texture2DArray.ArraySize = 1;
	layerViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	ppRTVs = new ID3D11RenderTargetView * [N_LAYER];

	ppSRVs = new ID3D11ShaderResourceView * [N_LAYER];

	for (int i = 0; i < N_LAYER; ++i)
	{
		layerViewDesc.Texture2DArray.FirstArraySlice = i;

		result = device->CreateRenderTargetView(m_renderTargetTexture, &layerViewDesc, &ppRTVs[i]);
		if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'LayerTexture2DArrayRenderTargetView'", L"Error", MB_OK | MB_ICONERROR); }

		//result = device->CreateShaderResourceView(m_renderTargetTexture, NULL, &ppSRVs[i]);
		//if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'LayerTexture2DArrayShaderResourceView'", L"Error", MB_OK | MB_ICONERROR); }

	}

	result = device->CreateShaderResourceView(m_renderTargetTexture, NULL, &ppSRVs[0]);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'LayerTexture2DArrayShaderResourceView'", L"Error", MB_OK | MB_ICONERROR); }



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

	result = device->CreateTexture2D(&textureDesc_, NULL, &m_depthStencilTexture);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Texture2D'", L"Error", MB_OK | MB_ICONERROR); }

	/// Create the depth stencil view desc
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	depthStencilViewDesc.Texture2D.MipSlice = 0;
	result = device->CreateDepthStencilView(m_depthStencilTexture, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Depth Stencil View'", L"Error", MB_OK | MB_ICONERROR); }


	return true;
}


void RenderTexture::Shutdown()
{
	if(m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = 0;
	}

	if (m_depthStencilTexture)
	{
		m_depthStencilTexture->Release();
		m_depthStencilTexture = 0;
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

	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}


	return;
}

void RenderTexture::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);

	return;
}

void RenderTexture::CreateMipmap(ID3D11DeviceContext* deviceContext)
{
	deviceContext->GenerateMips(m_shaderResourceView);
	return;
}

void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView,
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

	return;
}


ID3D11ShaderResourceView* RenderTexture::GetShaderResourceView()
{
	return m_shaderResourceView;
}

ID3D11RenderTargetView* RenderTexture::GetRenderTargetView()
{
	return m_renderTargetView;
}

ID3D11DepthStencilView* RenderTexture::GetDepthStencilView()
{
	return m_depthStencilView;
}


