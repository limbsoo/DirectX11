#include "Mesh.h"
#include <COMException.h>

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<MakeVertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures)
{
	this->deviceContext = deviceContext;
	this->textures = textures;

	HRESULT hr = this->vertexbuffer.Initialize(device, vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

	hr = this->indexbuffer.Initialize(device, indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
}

Mesh::Mesh(const Mesh& mesh)
{
	this->deviceContext = mesh.deviceContext;
	this->indexbuffer = mesh.indexbuffer;
	this->vertexbuffer = mesh.vertexbuffer;
	this->textures = mesh.textures;
}

void Mesh::Draw(ID3DX11EffectShaderResourceVariable* m_effect_texture, ID3DX11EffectTechnique* g_pTech, D3DX11_TECHNIQUE_DESC techDesc, ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout)
{
	UINT offset = NULL;

	for (int i = 0; i < textures.size(); i++)
	{
		if (textures[i].GetType() == aiTextureType::aiTextureType_DIFFUSE)
		{
			//this->deviceContext->PSSetShaderResources(0, 1, textures[i].GetTextureResourceViewAddress());
			m_effect_texture->SetResource(textures[i].GetTextureResourceView());
			break;
		}
	}

	g_pTech->GetPassByIndex(0)->Apply(0, this->deviceContext);
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexbuffer.GetAddressOf(), this->vertexbuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->IASetInputLayout(g_pLayout);
	g_pTech->GetDesc(&techDesc);
	this->deviceContext->DrawIndexed(this->indexbuffer.IndexCount(), 0, 0);

}

void Mesh::DrawFSQ(ID3DX11EffectShaderResourceVariable* m_effect_texture, ID3DX11EffectTechnique* g_pTech, D3DX11_TECHNIQUE_DESC techDesc, ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout)
{
	UINT offset = NULL;

	g_pTech->GetPassByIndex(0)->Apply(0, this->deviceContext);
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexbuffer.GetAddressOf(), this->vertexbuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->IASetInputLayout(g_pLayout);
	g_pTech->GetDesc(&techDesc);
	this->deviceContext->DrawIndexed(this->indexbuffer.IndexCount(), 0, 0);

}