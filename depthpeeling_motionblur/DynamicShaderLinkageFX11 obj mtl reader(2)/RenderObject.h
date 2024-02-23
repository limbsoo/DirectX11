#pragma once

#include<Model.h>
#include"define.h"
#include <d3dx11effect.h>

class RenderObject: public Model
{
public:
	RenderObject();
	RenderObject(const RenderObject&);

	float SetMovePosition(float sphereMovePosition, float displacement, float startPoint, float destinationPoint);
	void BeforeFrameRender(XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float scale, float xPos, float yPos, float zPos);
	void AfterFrameRender();
	void Render(ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout, ID3DX11EffectTechnique* g_pTech, D3DX11_TECHNIQUE_DESC techDesc, ID3DX11Effect* g_pEffect);
	void RenderFSQ(ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout, ID3DX11EffectTechnique* g_pTech, D3DX11_TECHNIQUE_DESC techDesc, ID3DX11Effect* g_pEffect);

	void RenderSkybox(ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout, ID3DX11EffectTechnique* g_pTech, D3DX11_TECHNIQUE_DESC techDesc, ID3DX11Effect* g_pEffect);
	void BeforeFrameRenderSkybox(XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float scale, float xPos, float yPos, float zPos);


	XMMATRIX m_scale;
	XMMATRIX m_rotation;
	XMMATRIX m_world;
	XMMATRIX m_worldView;
	XMMATRIX m_worldViewProjection;
	XMMATRIX m_previousWVP;
	XMMATRIX m_translation;

	ID3DX11EffectMatrixVariable* m_effect_worldViewProjection = NULL;
	ID3DX11EffectMatrixVariable* m_effect_PreviousWVP = NULL;
	ID3DX11EffectShaderResourceVariable* m_effect_texture = NULL;

	ID3DX11EffectMatrixVariable* m_effect_world = NULL;

	float xPos, yPos, zPos;

	float m_movePos_1;
	float m_movePos_2;
	float m_movePos_3;
	float m_movePos_4;

private:


};


