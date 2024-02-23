//***************************************************************************************
// RenderHelper.h by zhangbo0037 (C) 2016 All Rights Reserved.
// Helper classes for Render Object.
//***************************************************************************************

#ifndef RENDERHELPER_H
#define RENDERHELPER_H

#include "d3dx11effect.h"
#include "LightHelper.h"

ID3DX11Effect* g_pEffect = NULL;
ID3DX11EffectTechnique* g_pTech = NULL;
ID3DX11EffectMatrixVariable* g_W = NULL;
ID3DX11EffectMatrixVariable* g_WV = NULL;
ID3DX11EffectMatrixVariable* g_WVP = NULL;
ID3DX11EffectMatrixVariable* g_PreWVP = NULL;
ID3DX11EffectMatrixVariable* g_VTLP = NULL;
ID3DX11EffectMatrixVariable* g_FPS = NULL;
ID3DX11EffectVariable* g_DirectLight = NULL;
ID3DX11EffectVariable* g_PointLight = NULL;
ID3DX11EffectVariable* g_SpotLight = NULL;
ID3DX11EffectVariable* g_EyePos = NULL;
ID3DX11EffectVariable* g_Material = NULL;
ID3DX11EffectShaderResourceVariable* g_TextureArray = NULL;

ID3DX11EffectShaderResourceVariable* g_colorMap = NULL;
ID3DX11EffectShaderResourceVariable* g_motionVector3DMap = NULL;
ID3DX11EffectShaderResourceVariable* g_previousDepthMap = NULL;
ID3DX11EffectShaderResourceVariable* g_motionVector3DSquareMap = NULL;

ID3DX11EffectShaderResourceVariable* g_colorMap1 = NULL;
ID3DX11EffectShaderResourceVariable* g_motionVector3DMap1 = NULL;
ID3DX11EffectShaderResourceVariable* g_previousDepthMap1 = NULL;

ID3DX11EffectShaderResourceVariable* g_colorMap2 = NULL;
ID3DX11EffectShaderResourceVariable* g_motionVector3DMap2 = NULL;
ID3DX11EffectShaderResourceVariable* g_previousDepthMap2 = NULL;

ID3DX11EffectShaderResourceVariable* g_colorMap3 = NULL;
ID3DX11EffectShaderResourceVariable* g_motionVector3DMap3 = NULL;
ID3DX11EffectShaderResourceVariable* g_previousDepthMap3 = NULL;

ID3DX11EffectShaderResourceVariable* g_colorMap4 = NULL;
ID3DX11EffectShaderResourceVariable* g_motionVector3DMap4 = NULL;
ID3DX11EffectShaderResourceVariable* g_previousDepthMap4 = NULL;

ID3DX11EffectShaderResourceVariable* g_colorMap5 = NULL;
ID3DX11EffectShaderResourceVariable* g_motionVector3DMap5 = NULL;
ID3DX11EffectShaderResourceVariable* g_previousDepthMap5 = NULL;

ID3DX11EffectShaderResourceVariable* g_colorMap6 = NULL;
ID3DX11EffectShaderResourceVariable* g_motionVector3DMap6 = NULL;
ID3DX11EffectShaderResourceVariable* g_previousDepthMap6 = NULL;

ID3DX11EffectShaderResourceVariable* g_colorMapCur = NULL;
ID3DX11EffectShaderResourceVariable* g_motionVector3DMapCur = NULL;
ID3DX11EffectShaderResourceVariable* g_motionVector3DSquareMapCur = NULL;
ID3DX11EffectShaderResourceVariable* g_previousDepthMapCur = NULL;
ID3DX11EffectShaderResourceVariable* g_FullscreenQuad = NULL;
ID3DX11EffectShaderResourceVariable* g_FullscreenQuadDenoising = NULL;
D3DX11_TECHNIQUE_DESC                   techDesc;

ID3DX11EffectShaderResourceVariable* g_Scene = NULL;
ID3DX11EffectShaderResourceVariable* g_MSAAScene[KBUFFER_PASSES];

void GetEffect()
{
	g_W = g_pEffect->GetVariableByName("g_mWorld")->AsMatrix();
	g_WV = g_pEffect->GetVariableByName("g_mWorldView")->AsMatrix();
	g_WVP = g_pEffect->GetVariableByName("g_mWorldViewProjection")->AsMatrix();
	g_PreWVP = g_pEffect->GetVariableByName("g_mPreviousWorldViewProjection")->AsMatrix();
	g_DirectLight = g_pEffect->GetVariableByName("gDirLight");
	g_PointLight = g_pEffect->GetVariableByName("gPointLight");
	g_SpotLight = g_pEffect->GetVariableByName("gSpotLight");
	g_EyePos = g_pEffect->GetVariableByName("gEyePosW");
	g_Material = g_pEffect->GetVariableByName("gMaterial");

	g_TextureArray = g_pEffect->GetVariableByName("g_txDiffuse")->AsShaderResource();
	g_colorMap = g_pEffect->GetVariableByName("g_ColorMap")->AsShaderResource();
	g_motionVector3DMap = g_pEffect->GetVariableByName("g_motionVector3DMap")->AsShaderResource();
	g_previousDepthMap = g_pEffect->GetVariableByName("g_previousDepthMap")->AsShaderResource();
	g_motionVector3DSquareMap = g_pEffect->GetVariableByName("g_motionVector3DSquareMap")->AsShaderResource();

	g_colorMap1 = g_pEffect->GetVariableByName("g_ColorMap1")->AsShaderResource();
	g_motionVector3DMap1 = g_pEffect->GetVariableByName("g_motionVector3DMap1")->AsShaderResource();
	g_previousDepthMap1 = g_pEffect->GetVariableByName("g_previousDepthMap1")->AsShaderResource();

	g_colorMap2 = g_pEffect->GetVariableByName("g_ColorMap2")->AsShaderResource();
	g_motionVector3DMap2 = g_pEffect->GetVariableByName("g_motionVector3DMap2")->AsShaderResource();
	g_previousDepthMap2 = g_pEffect->GetVariableByName("g_previousDepthMap2")->AsShaderResource();

	g_colorMap3 = g_pEffect->GetVariableByName("g_ColorMap3")->AsShaderResource();
	g_motionVector3DMap3 = g_pEffect->GetVariableByName("g_motionVector3DMap3")->AsShaderResource();
	g_previousDepthMap3 = g_pEffect->GetVariableByName("g_previousDepthMap3")->AsShaderResource();

	g_colorMap4 = g_pEffect->GetVariableByName("g_ColorMap4")->AsShaderResource();
	g_motionVector3DMap4 = g_pEffect->GetVariableByName("g_motionVector3DMap4")->AsShaderResource();
	g_previousDepthMap4 = g_pEffect->GetVariableByName("g_previousDepthMap4")->AsShaderResource();

	g_colorMap5 = g_pEffect->GetVariableByName("g_ColorMap5")->AsShaderResource();
	g_motionVector3DMap5 = g_pEffect->GetVariableByName("g_motionVector3DMap5")->AsShaderResource();
	g_previousDepthMap5 = g_pEffect->GetVariableByName("g_previousDepthMap5")->AsShaderResource();

	g_colorMap6 = g_pEffect->GetVariableByName("g_ColorMap6")->AsShaderResource();
	g_motionVector3DMap6 = g_pEffect->GetVariableByName("g_motionVector3DMap6")->AsShaderResource();
	g_previousDepthMap6 = g_pEffect->GetVariableByName("g_previousDepthMap6")->AsShaderResource();

	g_colorMapCur = g_pEffect->GetVariableByName("g_ColorMapCur")->AsShaderResource();
	g_motionVector3DMapCur = g_pEffect->GetVariableByName("g_motionVector3DMapCur")->AsShaderResource();
	g_motionVector3DSquareMapCur = g_pEffect->GetVariableByName("g_motionVector3DSquareMapCur")->AsShaderResource();
	g_previousDepthMapCur = g_pEffect->GetVariableByName("g_previousDepthMapCur")->AsShaderResource();
	g_FullscreenQuad = g_pEffect->GetVariableByName("g_FullscreenQuad")->AsShaderResource();
	g_FullscreenQuadDenoising = g_pEffect->GetVariableByName("g_FullscreenQuadDenoising")->AsShaderResource();

	g_Scene = g_pEffect->GetVariableByName("g_Scene")->AsShaderResource();

	g_MSAAScene[0] = g_pEffect->GetVariableByName("g_MSAAScene")->AsShaderResource();
	if (KBUFFER_PASSES > 1) g_MSAAScene[1] = g_pEffect->GetVariableByName("g_MSAAScene1")->AsShaderResource();
}

void SetEffect(D3DXMATRIX World, D3DXMATRIX WorldViewMatrix, D3DXMATRIX WorldViewProjection, ID3D11ShaderResourceView* Textures, D3DXMATRIX previousWorldViewProjection)
{
	g_WVP->SetMatrix(WorldViewProjection);
	g_PreWVP->SetMatrix(previousWorldViewProjection);
	g_W->SetMatrix(World);
	g_WV->SetMatrix(WorldViewMatrix);
	g_TextureArray->SetResource(Textures);
}

void RenderObject(Animation* Model, ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* Layout)
{
	UINT stride = sizeof(MyVertex);
	UINT offset = NULL;
	ID3D11Buffer* pVertexBuffer = Model->GetVB();
	ID3D11Buffer* pIndexBuffer = Model->GetIB();
	ID3D11Buffer* pBuffer[1] = { pVertexBuffer };
	pd3dImmediateContext->IASetVertexBuffers(0, 1, pBuffer, &stride, &offset);
	pd3dImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pd3dImmediateContext->IASetInputLayout(Layout);

	g_pTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; p++)
	{
		g_pTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);

		vector<MeshEntry> Mesh = Model->GetMesh();
		UINT MeshCount = Mesh.size();
		for (UINT index = 0; index < MeshCount; index++)
		{
			pd3dImmediateContext->DrawIndexed(Mesh[index].NumIndices, Mesh[index].BaseIndex, Mesh[index].BaseVertex);
		}
	}
}



#endif // RENDERHELPER_H