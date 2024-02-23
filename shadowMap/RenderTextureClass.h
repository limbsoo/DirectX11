#pragma once

class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D11Device*, int, int, float, float);
	void Shutdown();
	void SetRenderTarget(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha);

	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	int m_textureWidth, m_textureHeight;

	ID3D11Texture2D* m_renderTargetTexture = nullptr;;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;;
	ID3D11ShaderResourceView* m_shaderResourceView = nullptr;;

	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_orthoMatrix;
};
