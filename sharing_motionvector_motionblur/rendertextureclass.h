////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERTEXTURECLASS_H_
#define _RENDERTEXTURECLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: RenderTextureClass
////////////////////////////////////////////////////////////////////////////////
class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D11Device*, int, int);
	bool InitializeBackBufferWithMipMap(ID3D11Device*, int, int);
	bool SetMSAAInitialize(ID3D11Device*, int, int);
	bool SetInitialize(ID3D11Device*, int, int);

	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void Clear(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);

	void CreateMipmap(ID3D11DeviceContext*);

	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11RenderTargetView* GetRenderTargetView();
	ID3D11DepthStencilView* GetDepthStencilView();

	ID3D11DepthStencilState* GetWriteDepthStencilState();
	ID3D11DepthStencilState* GetDecrementDepthStencilState();
	ID3D11RasterizerState* GetRasterizerState();
	ID3D11BlendState* GetBlendState();
	ID3D11Query* GetQuery();

private:
	ID3D11Texture2D* m_renderTargetTexture;

	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	ID3D11Texture2D* m_renderTargetTextureDSV;
	ID3D11DepthStencilView* m_depthStencilView;

	ID3D11DepthStencilState* m_WriteDepthStencilState;
	ID3D11DepthStencilState* m_DecrementdepthStencilState;

	ID3D11RasterizerState* m_pRasterState;
	ID3D11BlendState* m_pBlendState;
	ID3D11Query* m_pQuery;
	DXGI_SWAP_CHAIN_DESC* swapChainDesc;
};

#endif