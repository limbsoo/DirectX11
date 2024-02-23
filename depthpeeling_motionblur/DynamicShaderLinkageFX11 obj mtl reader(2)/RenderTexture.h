#ifndef _RenderTexture_H_
#define _RenderTexture_H_

#include <d3d11.h>
#include"define.h"
#include <atlstr.h>

typedef
enum { RENDER_TARGET, RENDER_TARGET_ARRAY, RENDER_TARGET_MSAA, RENDER_TARGET_MIPMAP } RenderTargetType;

class RenderTexture
{
public:
	RenderTexture();
	RenderTexture(const RenderTexture&);
	~RenderTexture();

	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void CreateMipmap(ID3D11DeviceContext*);

	bool Initialize(ID3D11Device* pDevice, RenderTargetType type);
	bool Initialize(ID3D11Device*);
	bool RenderTexture::InitializeBackBufferWithMipMap_Array(ID3D11Device* device, int nNumArr);
	bool Initialize_DepthBuffer_DepthPeeling(ID3D11Device* pdevice);

	ID3D11ShaderResourceView*	GetShaderResourceView();
	ID3D11RenderTargetView*		GetRenderTargetView();
	ID3D11DepthStencilView*		GetDepthStencilView();

	ID3D11ShaderResourceView* GetShaderResourceViewArray(int i);
	ID3D11RenderTargetView* GetRenderTargetViewArray(int i);

	ID3D11DepthStencilState* GetWriteDepthStencilState();
	ID3D11DepthStencilState* GetUseDepthStencilState();
	ID3D11DepthStencilState* GetUseDepthTestDSS();

	ID3D11RasterizerState* GetRasterizerState();

	ID3D11BlendState* GetBlendState();

	ID3D11RenderTargetView** ppRTVs;
	ID3D11ShaderResourceView** ppSRVs;

private:

	IDXGISwapChain*				m_swapChain = nullptr;
	ID3D11Device*				m_device = nullptr;
	ID3D11DeviceContext*		m_deviceContext = nullptr;

	ID3D11Texture2D*			m_renderTargetTexture;
	ID3D11Texture2D*			m_depthStencilTexture;

	ID3D11RenderTargetView*		m_renderTargetView;
	ID3D11DepthStencilView*		m_depthStencilView;
	ID3D11ShaderResourceView*	m_shaderResourceView;

	ID3D11RenderTargetView** m_renderTargetViewArray = nullptr;
	ID3D11ShaderResourceView** m_shaderResourceViewArray = nullptr;

	ID3D11DepthStencilState* m_pDSWrite = nullptr;
	ID3D11DepthStencilState* m_pDSUse = nullptr;
	ID3D11DepthStencilState* m_pUseDepthTest = nullptr;
	ID3D11RasterizerState* m_pRS = nullptr;
	ID3D11BlendState* m_pBS = nullptr;
};

#endif