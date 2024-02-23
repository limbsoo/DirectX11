//***************************************************************************************
// RenderHelper.h by zhangbo0037 (C) 2016 All Rights Reserved.
// Helper classes for Render Object.
//***************************************************************************************


//#include"define.h"
//
//#include "d3dx11effect.h"
//
//
//ID3DX11Effect* g_pEffect = NULL;
//ID3DX11EffectTechnique* g_pTech = NULL;
//ID3DX11EffectMatrixVariable* e_W = NULL;
//ID3DX11EffectMatrixVariable* e_WV = NULL;
//ID3DX11EffectMatrixVariable* e_WVP = NULL;
//ID3DX11EffectMatrixVariable* e_PreWVP = NULL;
//
//ID3DX11EffectShaderResourceVariable* e_TextureArray = NULL;
//
//ID3DX11EffectShaderResourceVariable* e_colorMap_SRV;
//ID3DX11EffectShaderResourceVariable* e_motionVectorMap_SRV;
//ID3DX11EffectShaderResourceVariable* e_motionVectorSquareMap_SRV;
//ID3DX11EffectShaderResourceVariable* e_depthBuffer_SRV;
//
//ID3DX11EffectShaderResourceVariable* e_motionBlur_SRV = NULL;
//ID3DX11EffectShaderResourceVariable* e_denoising_SRV = NULL;
//
//D3DX11_TECHNIQUE_DESC techDesc;
//
//
//void GetEffect()
//{
//	e_W = g_pEffect->GetVariableByName("g_world")->AsMatrix();
//	e_WV = g_pEffect->GetVariableByName("g_worldView")->AsMatrix();
//	e_WVP = g_pEffect->GetVariableByName("g_worldViewProjection")->AsMatrix();
//	e_PreWVP = g_pEffect->GetVariableByName("g_previousWorldViewProjection")->AsMatrix();
//	e_TextureArray = g_pEffect->GetVariableByName("g_txDiffuse")->AsShaderResource();
//
//	e_colorMap_SRV = g_pEffect->GetVariableByName("g_colorMap")->AsShaderResource();
//	e_motionVectorMap_SRV = g_pEffect->GetVariableByName("g_motionVectorMap")->AsShaderResource();
//	e_motionVectorSquareMap_SRV = g_pEffect->GetVariableByName("g_motionVectorSquareMap")->AsShaderResource();
//	e_depthBuffer_SRV = g_pEffect->GetVariableByName("g_depthBuffer")->AsShaderResource();
//	e_motionBlur_SRV = g_pEffect->GetVariableByName("g_motionBlur")->AsShaderResource();
//	e_denoising_SRV = g_pEffect->GetVariableByName("g_denosing")->AsShaderResource();
//
//}
//
//
//void SetEffect(XMMATRIX World, XMMATRIX WorldViewMatrix, XMMATRIX WorldViewProjection, ID3D11ShaderResourceView* Textures, XMMATRIX previousWorldViewProjection)
//{
//	XMFLOAT4X4 tmp4x4;
//	XMStoreFloat4x4(&tmp4x4, WorldViewProjection);
//	e_WVP->SetMatrix(reinterpret_cast<float*>(&tmp4x4));
//
//	XMStoreFloat4x4(&tmp4x4, previousWorldViewProjection);
//	e_PreWVP->SetMatrix(reinterpret_cast<float*>(&tmp4x4));
//
//	XMStoreFloat4x4(&tmp4x4, World);
//	e_W->SetMatrix(reinterpret_cast<float*>(&tmp4x4));
//
//	XMStoreFloat4x4(&tmp4x4, WorldViewMatrix);
//	e_WV->SetMatrix(reinterpret_cast<float*>(&tmp4x4));
//
//	e_TextureArray->SetResource(Textures);
//}
//
//void RenderObject(RenderedObjectClass* Model, ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* Layout)
//{
//	UINT stride = sizeof(MyVertex);
//	UINT offset = NULL;
//	ID3D11Buffer* pVertexBuffer = Model->GetVB();
//	ID3D11Buffer* pIndexBuffer = Model->GetIB();
//	ID3D11Buffer* pBuffer[1] = { pVertexBuffer };
//	pd3dImmediateContext->IASetVertexBuffers(0, 1, pBuffer, &stride, &offset);
//	pd3dImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
//	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	pd3dImmediateContext->IASetInputLayout(Layout);
//
//	g_pTech->GetDesc(&techDesc);
//
//	for (UINT p = 0; p < techDesc.Passes; p++)
//	{
//		g_pTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
//
//		vector<MeshEntry> Mesh = Model->GetMesh();
//		UINT MeshCount = Mesh.size();
//		for (UINT index = 0; index < MeshCount; index++)
//		{
//			pd3dImmediateContext->DrawIndexed(Mesh[index].NumIndices, Mesh[index].BaseIndex, Mesh[index].BaseVertex);
//		}
//	}
//
//}
//
//void Render(ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout, RenderedObjectClass* Model_1, RenderedObjectClass* Model_2, RenderedObjectClass* Model_3, RenderedObjectClass* Model_4)
//{
//	GetEffect();
//
//	SetEffect(Model_1->m_world, Model_1->m_worldView, Model_1->m_worldViewProjection, Model_1->m_texture, Model_1->m_previousMVP);
//	RenderObject(Model_1, pd3dImmediateContext, g_pLayout);
//
//	SetEffect(Model_2->m_world, Model_2->m_worldView, Model_2->m_worldViewProjection, Model_2->m_texture, Model_2->m_previousMVP);
//	RenderObject(Model_2, pd3dImmediateContext, g_pLayout);
//
//	SetEffect(Model_3->m_world, Model_3->m_worldView, Model_3->m_worldViewProjection, Model_3->m_texture, Model_3->m_previousMVP);
//	RenderObject(Model_3, pd3dImmediateContext, g_pLayout);
//
//	SetEffect(Model_4->m_world, Model_4->m_worldView, Model_4->m_worldViewProjection, Model_4->m_texture, Model_4->m_previousMVP);
//	RenderObject(Model_4, pd3dImmediateContext, g_pLayout);
//}


