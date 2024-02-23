#include "stdafx.h"
#include "LightClass.h"


LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}


void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);
}


void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
}


void LightClass::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}


void LightClass::SetLookAt(float x, float y, float z)
{
	m_lookAt = XMFLOAT3(x, y, z);
}


XMFLOAT4 LightClass::GetAmbientColor()
{
	return m_ambientColor;
}


XMFLOAT4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


XMFLOAT3 LightClass::GetPosition()
{
	return m_position;
}


void LightClass::GenerateViewMatrix()
{
	// 위쪽을 가리키는 벡터를 설정합니다.
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	XMVECTOR upVector = XMLoadFloat3(&up);
	XMVECTOR positionVector = XMLoadFloat3(&m_position);
	XMVECTOR lookAtVector = XMLoadFloat3(&m_lookAt);

	// 세 벡터로부터 뷰 행렬을 만듭니다.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

}


void LightClass::GenerateProjectionMatrix(float SCREEN_NEAR, float SCREEN_FAR)
{
	// 정사각형 광원에 대한 시야 및 화면 비율을 설정합니다.
	float fieldOfView = (float)XM_PI / 2.0f;
	float screenAspect = 1.0f;

	// 빛의 투영 행렬을 만듭니다.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, SCREEN_NEAR, SCREEN_FAR);
}


void LightClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}


void LightClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}