#ifndef _RenderTexture_H_
#define _RenderTexture_H_

#include <d3d11.h>
#include"define.h"
#include <atlstr.h>

class RenderTexture
{
public:
	RenderTexture();
	RenderTexture(const RenderTexture&);
	~RenderTexture();

	void Shutdown();
	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void CreateMipmap(ID3D11DeviceContext*);

	bool Initialize(ID3D11Device*, int, int);
	bool InitializeBackBufferWithMipMap_Array(ID3D11Device*, int, int);
	bool Initialize_DepthBuffer_DepthPeeling(ID3D11Device* pdevice, int textureWidth, int textureHeight);

	ID3D11ShaderResourceView*	GetShaderResourceView();
	ID3D11RenderTargetView*		GetRenderTargetView();
	ID3D11DepthStencilView*		GetDepthStencilView();

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






};

#endif