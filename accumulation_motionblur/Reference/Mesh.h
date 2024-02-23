#pragma once
#include "MakeVertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include <Importer.hpp>
#include <postprocess.h>
#include <scene.h>

#include <Texture.h>
#include <d3dx11effect.h>

class Mesh
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<MakeVertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures);
	Mesh(const Mesh& mesh);
	void Draw(ID3DX11EffectShaderResourceVariable* m_effect_texture, ID3DX11EffectTechnique* g_pTech, D3DX11_TECHNIQUE_DESC techDesc, ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout);
	void DrawFSQ(ID3DX11EffectShaderResourceVariable* m_effect_texture, ID3DX11EffectTechnique* g_pTech, D3DX11_TECHNIQUE_DESC techDesc, ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout);

private:
	VertexBuffer<MakeVertex> vertexbuffer;
	IndexBuffer indexbuffer;
	ID3D11DeviceContext* deviceContext;
	std::vector<Texture> textures;
};