//***************************************************************************************
// RenderHelper.h by zhangbo0037 (C) 2016 All Rights Reserved.
// Helper classes for Render Object.
//***************************************************************************************
#include"define.h"

#ifndef RENDERHELPER_H
#define RENDERHELPER_H

#include "d3dx11effect.h"
#include "LightHelper.h"

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

ID3DX11Effect* g_pEffect = NULL;

ID3DX11EffectTechnique* g_pRendering_Tech = NULL;
ID3DX11EffectTechnique* g_pMotionBlur_Tech = NULL;

ID3DX11EffectMatrixVariable* g_WVP = NULL;
ID3DX11EffectMatrixVariable* g_PreWVP = NULL;

ID3DX11EffectShaderResourceVariable* g_Texture = NULL;

D3DX11_TECHNIQUE_DESC                techDesc;

ID3DX11EffectShaderResourceVariable* g_1at8Frame;
ID3DX11EffectShaderResourceVariable* g_9at16Frame;
ID3DX11EffectShaderResourceVariable* g_17at24Frame;
ID3DX11EffectShaderResourceVariable* g_25at32Frame;


void GetEffect()
{
	g_WVP = g_pEffect->GetVariableByName("g_mWorldViewProjection")->AsMatrix();
	g_PreWVP = g_pEffect->GetVariableByName("g_mPreviousWorldViewProjection")->AsMatrix();

	g_Texture = g_pEffect->GetVariableByName("g_modelTexture")->AsShaderResource();

	g_1at8Frame = g_pEffect->GetVariableByName("g_1at8Frame")->AsShaderResource();
	g_9at16Frame = g_pEffect->GetVariableByName("g_9at16Frame")->AsShaderResource();
	g_17at24Frame = g_pEffect->GetVariableByName("g_17at24Frame")->AsShaderResource();
	g_25at32Frame = g_pEffect->GetVariableByName("g_25at32Frame")->AsShaderResource();
}

void SetEffect(XMMATRIX WorldViewProjection, ID3D11ShaderResourceView* Textures, XMMATRIX previousWorldViewProjection)
{
	XMFLOAT4X4 tmp4x4;
	XMStoreFloat4x4(&tmp4x4, WorldViewProjection);
	g_WVP->SetMatrix(reinterpret_cast<float*>(&tmp4x4));

	XMStoreFloat4x4(&tmp4x4, previousWorldViewProjection);
	g_PreWVP->SetMatrix(reinterpret_cast<float*>(&tmp4x4));

	g_Texture->SetResource(Textures);
}

void RenderObject(Animation* Model, ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* Layout, ID3DX11EffectTechnique* pTech)
{
	constexpr UINT stride = sizeof(MyVertex);
	constexpr UINT offset = NULL;
	ID3D11Buffer* pVertexBuffer = Model->GetVB();
	ID3D11Buffer* pIndexBuffer = Model->GetIB();
	ID3D11Buffer* pBuffer[1] = { pVertexBuffer };
	pd3dImmediateContext->IASetVertexBuffers(0, 1, pBuffer, &stride, &offset);
	pd3dImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dImmediateContext->IASetInputLayout(Layout);

	pTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; p++)
	{
		pTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);

		vector<MeshEntry> Mesh = Model->GetMesh();
		const UINT MeshCount = Mesh.size();
		for (UINT index = 0; index < MeshCount; index++)
		{
			pd3dImmediateContext->DrawIndexed(Mesh[index].NumIndices, Mesh[index].BaseIndex, Mesh[index].BaseVertex);
		}
	}
}


#endif //RENDERHELPER_H
