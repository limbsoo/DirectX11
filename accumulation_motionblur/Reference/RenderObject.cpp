#include<RenderObject.h>

RenderObject::RenderObject()
{
}

RenderObject::RenderObject(const RenderObject& other)
{
}

void RenderObject::BeforeFrameRender(XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float scale, float xPos, float yPos, float zPos)
{
	m_translation = XMMatrixTranslation(xPos, yPos, zPos);
	m_scale = XMMatrixScaling(scale, scale, scale);
	m_rotation = XMMatrixRotationY(XMConvertToRadians(0.f));
	m_world = m_scale * m_translation;
	m_worldView = m_world * viewMatrix;
	m_worldViewProjection = m_world * viewMatrix * projectionMatrix;
}

void RenderObject::AfterFrameRender()
{
	m_previousWVP = m_worldViewProjection;
}


float RenderObject::SetMovePosition(float sphereMovePosition, float displacement, float startPoint, float destinationPoint)
{
	if (startPoint < destinationPoint)
	{
		if (sphereMovePosition <= destinationPoint)
		{
			sphereMovePosition += displacement;
		}
		
		else
		{
			sphereMovePosition = startPoint;
		}
	}

	else
	{
		if (sphereMovePosition >= destinationPoint)
		{
			sphereMovePosition -= displacement;
		}

		else
		{
			sphereMovePosition = startPoint;
		}

	}

	return sphereMovePosition;
}


void RenderObject::Render(ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout, ID3DX11EffectTechnique* g_pTech, D3DX11_TECHNIQUE_DESC techDesc, ID3DX11Effect* g_pEffect)
{
	m_effect_world = g_pEffect->GetVariableByName("g_world")->AsMatrix();
	m_effect_worldView = g_pEffect->GetVariableByName("g_worldView")->AsMatrix();
	m_effect_worldViewProjection = g_pEffect->GetVariableByName("g_worldViewProjection")->AsMatrix();
	m_effect_PreviousWVP = g_pEffect->GetVariableByName("g_previousWorldViewProjection")->AsMatrix();
	m_effect_texture = g_pEffect->GetVariableByName("g_txDiffuse")->AsShaderResource();

	XMFLOAT4X4 tmp4x4;
	XMStoreFloat4x4(&tmp4x4, m_worldViewProjection);
	m_effect_worldViewProjection->SetMatrix(reinterpret_cast<float*>(&tmp4x4));
	XMStoreFloat4x4(&tmp4x4, m_previousWVP);
	m_effect_PreviousWVP->SetMatrix(reinterpret_cast<float*>(&tmp4x4));
	XMStoreFloat4x4(&tmp4x4, m_world);
	m_effect_world->SetMatrix(reinterpret_cast<float*>(&tmp4x4));
	XMStoreFloat4x4(&tmp4x4, m_worldView);
	m_effect_worldView->SetMatrix(reinterpret_cast<float*>(&tmp4x4));

	g_pTech->GetDesc(&techDesc);
	pd3dImmediateContext->IASetInputLayout(g_pLayout);
	Draw(m_effect_texture, g_pTech, techDesc , pd3dImmediateContext, g_pLayout);

}

void RenderObject::RenderFSQ(ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout, ID3DX11EffectTechnique* g_pTech, D3DX11_TECHNIQUE_DESC techDesc, ID3DX11Effect* g_pEffect)
{
	g_pTech->GetDesc(&techDesc);
	pd3dImmediateContext->IASetInputLayout(g_pLayout);
	DrawFSQ(m_effect_texture, g_pTech, techDesc, pd3dImmediateContext, g_pLayout);
}