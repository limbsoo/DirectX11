////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "rendertextureclass.h"
//#include "SharedDefines.h"
#include "define.h"
#include <d3dx11effect.h>

RenderTextureClass::RenderTextureClass() noexcept
{
}

RenderTextureClass::RenderTextureClass(const RenderTextureClass& other) noexcept
{
}

RenderTextureClass::~RenderTextureClass() noexcept
{
}

bool RenderTextureClass::Initialize(ID3D11Device* pDevice, int width, int height, RenderTargetType type)
{
	HRESULT result;

	if (type == RENDER_TARGET) {
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		result = pDevice->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
		if (FAILED(result))
		{
			return false;
		}

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		result = pDevice->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
		if (FAILED(result))
		{
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = -1;

		result = pDevice->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);

		if (FAILED(result))
		{
			return false;
		}

		D3D11_TEXTURE2D_DESC textureDesc_;
		ZeroMemory(&textureDesc_, sizeof(textureDesc_));
		textureDesc_.Width = width;
		textureDesc_.Height = height;
		textureDesc_.MipLevels = 0;
		textureDesc_.ArraySize = 1;
		textureDesc_.Format = DXGI_FORMAT_R24G8_TYPELESS;
		textureDesc_.SampleDesc.Count = 1;
		textureDesc_.SampleDesc.Quality = 0;
		textureDesc_.Usage = D3D11_USAGE_DEFAULT;
		textureDesc_.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		textureDesc_.CPUAccessFlags = 0;
		textureDesc_.MiscFlags = 0;

		result = pDevice->CreateTexture2D(&textureDesc_, NULL, &m_renderTargetTextureDSV);
		if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Texture2D'", L"Error", MB_OK | MB_ICONERROR); }

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		result = pDevice->CreateDepthStencilView(m_renderTargetTextureDSV, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Depth Stencil View'", L"Error", MB_OK | MB_ICONERROR); }
	}

	else if (type == RENDER_TARGET_ARRAY) {

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = TEXTUREARRSIZE;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		result = pDevice->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
		if (FAILED(result))
		{
			return false;
		}

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));

		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		renderTargetViewDesc.Texture2DArray.ArraySize = 1;

		m_renderTargetViewArray = new ID3D11RenderTargetView * [textureDesc.ArraySize];

		for (UINT i = 0; i < textureDesc.ArraySize; ++i)
		{
			renderTargetViewDesc.Texture2DArray.FirstArraySlice = i;
			result = pDevice->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetViewArray[i]);

			if (FAILED(result))
			{
				return false;
			}
		}

		m_shaderResourceViewArray = new ID3D11ShaderResourceView * [textureDesc.ArraySize];

		for (UINT i = 0; i < textureDesc.ArraySize; ++i)
		{
			result = pDevice->CreateShaderResourceView(m_renderTargetTexture, NULL, &m_shaderResourceViewArray[i]);

			if (FAILED(result))
			{
				return false;
			}
		}

		D3D11_TEXTURE2D_DESC textureDesc_;
		ZeroMemory(&textureDesc_, sizeof(textureDesc_));
		textureDesc_.Width = width;
		textureDesc_.Height = height;
		textureDesc_.MipLevels = 0;
		textureDesc_.ArraySize = 1;
		textureDesc_.Format = DXGI_FORMAT_R24G8_TYPELESS;
		textureDesc_.SampleDesc.Count = 1;
		textureDesc_.SampleDesc.Quality = 0;
		textureDesc_.Usage = D3D11_USAGE_DEFAULT;
		textureDesc_.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		textureDesc_.CPUAccessFlags = 0;
		textureDesc_.MiscFlags = 0;

		result = pDevice->CreateTexture2D(&textureDesc_, NULL, &m_renderTargetTextureDSV);
		if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Texture2D'", L"Error", MB_OK | MB_ICONERROR); }

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		result = pDevice->CreateDepthStencilView(m_renderTargetTextureDSV, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Depth Stencil View'", L"Error", MB_OK | MB_ICONERROR); }
	}

	else if (type == RENDER_TARGET_MSAA) {
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
		textureDesc.SampleDesc.Count = MSAA_LEVEL;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		UINT msaa_quality;
		result = pDevice->CheckMultisampleQualityLevels(textureDesc.Format, MSAA_LEVEL, &msaa_quality);
		msaa_quality--;
		textureDesc.SampleDesc.Quality = msaa_quality;

		result = pDevice->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
		if (FAILED(result))
		{
			return false;
		}
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));

		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

		result = pDevice->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
		if (FAILED(result))
		{
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));

		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

		result = pDevice->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);

		if (FAILED(result))
		{
			return false;
		}

		D3D11_TEXTURE2D_DESC textureDesc_;
		ZeroMemory(&textureDesc_, sizeof(textureDesc_));
		textureDesc_.Width = width;
		textureDesc_.Height = height;
		textureDesc_.MipLevels = 1;
		textureDesc_.ArraySize = 1;
		textureDesc_.Format = DXGI_FORMAT_R24G8_TYPELESS;
		textureDesc_.SampleDesc.Count = MSAA_LEVEL;
		textureDesc_.Usage = D3D11_USAGE_DEFAULT;
		textureDesc_.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		textureDesc_.CPUAccessFlags = 0;
		textureDesc_.MiscFlags = 0;
		textureDesc_.SampleDesc.Quality = msaa_quality;

		result = pDevice->CreateTexture2D(&textureDesc_, NULL, &m_renderTargetTextureDSV);
		if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Texture2D'", L"Error", MB_OK | MB_ICONERROR); }

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		result = pDevice->CreateDepthStencilView(m_renderTargetTextureDSV, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Depth Stencil View'", L"Error", MB_OK | MB_ICONERROR); }

		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(dsDesc));
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

		pDevice->CreateDepthStencilState(&dsDesc, &m_pDSWrite);

		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
		//dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		//dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_DECR_SAT;
		//dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR_SAT;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_DECR_SAT;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR_SAT;

		pDevice->CreateDepthStencilState(&dsDesc, &m_pDSUse);

		D3D11_RASTERIZER_DESC rsDesc;
		ZeroMemory(&rsDesc, sizeof(rsDesc));
		rsDesc.AntialiasedLineEnable = false;
		rsDesc.DepthBias = 0;
		rsDesc.DepthBiasClamp = 0.0f;
		rsDesc.FrontCounterClockwise = false;
		rsDesc.ScissorEnable = false;
		rsDesc.SlopeScaledDepthBias = 0.0f;

		rsDesc.CullMode = D3D11_CULL_BACK;
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.MultisampleEnable = false;


		pDevice->CreateRasterizerState(&rsDesc, &m_pRS);

		D3D11_BLEND_DESC bsDesc;
		ZeroMemory(&bsDesc, sizeof(D3D11_BLEND_DESC));
		pDevice->CreateBlendState(&bsDesc, &m_pBS);
	}

	else if (type == RENDER_TARGET_MIPMAP) {

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		result = pDevice->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
		if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Texture2D'", L"Error", MB_OK | MB_ICONERROR); }

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		result = pDevice->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
		if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Render Target View'", L"Error", MB_OK | MB_ICONERROR); }

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels = -1;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

		result = pDevice->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
		if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Shader Resource View'", L"Error", MB_OK | MB_ICONERROR); }

		D3D11_TEXTURE2D_DESC textureDesc_;
		ZeroMemory(&textureDesc_, sizeof(textureDesc_));
		textureDesc_.Width = width;
		textureDesc_.Height = height;
		textureDesc_.MipLevels = 1;
		textureDesc_.ArraySize = 1;
		textureDesc_.Format = DXGI_FORMAT_R24G8_TYPELESS;
		textureDesc_.SampleDesc.Count = 1;
		textureDesc_.SampleDesc.Quality = 0;
		textureDesc_.Usage = D3D11_USAGE_DEFAULT;
		textureDesc_.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		textureDesc_.CPUAccessFlags = 0;
		textureDesc_.MiscFlags = 0;

		result = pDevice->CreateTexture2D(&textureDesc_, NULL, &m_renderTargetTexture);
		if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Texture2D'", L"Error", MB_OK | MB_ICONERROR); }

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		result = pDevice->CreateDepthStencilView(m_renderTargetTexture, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(result)) { MessageBox(NULL, L"Error in generating 'Depth Stencil View'", L"Error", MB_OK | MB_ICONERROR); }

		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(dsDesc));

		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		pDevice->CreateDepthStencilState(&dsDesc, &m_pUseDepthTest);
	}
	return true;
}


void RenderTextureClass::Shutdown(RenderTargetType type)
{
	if (type == RENDER_TARGET) {
		if (m_renderTargetTexture)
		{
			m_renderTargetTexture->Release();
			m_renderTargetTexture = 0;
		}

		if (m_renderTargetView)
		{
			m_renderTargetView->Release();
			m_renderTargetView = 0;
		}

		if (m_shaderResourceView)
		{
			m_shaderResourceView->Release();
			m_shaderResourceView = 0;
		}

		if (m_renderTargetTextureDSV)
		{
			m_renderTargetTextureDSV->Release();
			m_renderTargetTextureDSV = 0;
		}

		if (m_depthStencilView)
		{
			m_depthStencilView->Release();
			m_depthStencilView = 0;
		}
	}

	else if (type == RENDER_TARGET_ARRAY) {
		if (m_renderTargetTexture)
		{
			m_renderTargetTexture->Release();
			m_renderTargetTexture = 0;
		}

		for (int i = 0; i < TEXTUREARRSIZE; i++) {
			if (m_renderTargetViewArray[i])
			{
				m_renderTargetViewArray[i]->Release();
				m_renderTargetViewArray[i] = 0;
			}

			if (m_shaderResourceViewArray[i])
			{
				m_shaderResourceViewArray[i]->Release();
				m_shaderResourceViewArray[i] = 0;
			}
		}

		if (m_renderTargetTextureDSV)
		{
			m_renderTargetTextureDSV->Release();
			m_renderTargetTextureDSV = 0;
		}

		if (m_depthStencilView)
		{
			m_depthStencilView->Release();
			m_depthStencilView = 0;
		}
	}

	else if (type == RENDER_TARGET_MSAA) {
		if (m_renderTargetTexture)
		{
			m_renderTargetTexture->Release();
			m_renderTargetTexture = 0;
		}

		if (m_renderTargetView)
		{
			m_renderTargetView->Release();
			m_renderTargetView = 0;
		}

		if (m_shaderResourceView)
		{
			m_shaderResourceView->Release();
			m_shaderResourceView = 0;
		}

		if (m_renderTargetTextureDSV)
		{
			m_renderTargetTextureDSV->Release();
			m_renderTargetTextureDSV = 0;
		}

		if (m_depthStencilView)
		{
			m_depthStencilView->Release();
			m_depthStencilView = 0;
		}
		if (m_pDSWrite)
		{
			m_pDSWrite->Release();
			m_pDSWrite = 0;
		}
		if (m_pDSUse)
		{
			m_pDSUse->Release();
			m_pDSUse = 0;
		}
		if (m_pRS)
		{
			m_pRS->Release();
			m_pRS = 0;
		}
		if (m_pBS)
		{
			m_pBS->Release();
			m_pBS = 0;
		}
	}

	else if (type == RENDER_TARGET_MIPMAP) {
		if (m_renderTargetTexture)
		{
			m_renderTargetTexture->Release();
			m_renderTargetTexture = 0;
		}

		if (m_renderTargetView)
		{
			m_renderTargetView->Release();
			m_renderTargetView = 0;
		}

		if (m_shaderResourceView)
		{
			m_shaderResourceView->Release();
			m_shaderResourceView = 0;
		}

		if (m_depthStencilView)
		{
			m_depthStencilView->Release();
			m_depthStencilView = 0;
		}
		if (m_pUseDepthTest)
		{
			m_pUseDepthTest->Release();
			m_pUseDepthTest = 0;
		}

	}
}

ID3D11ShaderResourceView* RenderTextureClass::GetShaderResourceView()
{
	return m_shaderResourceView;
}

ID3D11ShaderResourceView* RenderTextureClass::GetShaderResourceViewArray(int i)
{
	return m_shaderResourceViewArray[i];
}

ID3D11RenderTargetView* RenderTextureClass::GetRenderTargetViewArray(int i)
{
	return m_renderTargetViewArray[i];
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
	return m_pDSWrite;
}

ID3D11DepthStencilState* RenderTextureClass::GetUseDepthStencilState()
{
	return m_pDSUse;
}

ID3D11DepthStencilState* RenderTextureClass::GetUseDepthTestDSS()
{
	return m_pUseDepthTest;
}

ID3D11RasterizerState* RenderTextureClass::GetRasterizerState()
{
	return m_pRS;
}

ID3D11BlendState* RenderTextureClass::GetBlendState()
{
	return m_pBS;
}