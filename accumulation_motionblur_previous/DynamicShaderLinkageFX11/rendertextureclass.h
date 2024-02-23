#ifndef _RENDERTEXTURECLASS_H_
#define _RENDERTEXTURECLASS_H_

#include <d3d11.h>

#include <atlstr.h>

typedef
enum { RENDER_TARGET, RENDER_TARGET_ARRAY, RENDER_TARGET_MSAA, RENDER_TARGET_MIPMAP } RenderTargetType;

class RenderTextureClass
{
public:
	RenderTextureClass () noexcept;
	RenderTextureClass(const RenderTextureClass&) noexcept;
	~RenderTextureClass() noexcept;

public:
	void Shutdown(RenderTargetType type);
	bool Initialize(ID3D11Device* pDevice, int width, int height, RenderTargetType type);

public:
	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11RenderTargetView* GetRenderTargetView();
	ID3D11ShaderResourceView* GetShaderResourceViewArray(int i);
	ID3D11RenderTargetView* GetRenderTargetViewArray(int i);
	ID3D11DepthStencilView* GetDepthStencilView();

	ID3D11DepthStencilState* GetWriteDepthStencilState();
	ID3D11DepthStencilState* GetUseDepthStencilState();
	ID3D11DepthStencilState* GetUseDepthTestDSS();

	ID3D11RasterizerState* GetRasterizerState();

	ID3D11BlendState* GetBlendState();

private:
	ID3D11Texture2D* m_renderTargetTexture = nullptr;
	ID3D11Texture2D* m_renderTargetTextureDSV = nullptr;

	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11ShaderResourceView* m_shaderResourceView = nullptr;

	ID3D11RenderTargetView** m_renderTargetViewArray = nullptr;
	ID3D11ShaderResourceView** m_shaderResourceViewArray = nullptr;

	ID3D11DepthStencilView* m_depthStencilView = nullptr;

	ID3D11DepthStencilState* m_pDSWrite = nullptr;
	ID3D11DepthStencilState* m_pDSUse = nullptr;
	ID3D11DepthStencilState* m_pUseDepthTest = nullptr;
	ID3D11RasterizerState* m_pRS = nullptr;
	ID3D11BlendState* m_pBS = nullptr;
};

#endif