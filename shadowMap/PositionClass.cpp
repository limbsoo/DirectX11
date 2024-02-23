#include "stdafx.h"
#include "PositionClass.h"


PositionClass::PositionClass()
{
}


PositionClass::PositionClass(const PositionClass& other)
{
}


PositionClass::~PositionClass()
{
}


void PositionClass::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}


void PositionClass::SetRotation(float x, float y, float z)
{
	m_rotation = XMFLOAT3(x, y, z);
}


void PositionClass::GetPosition(XMFLOAT3& p)
{
	p = m_position;
}


void PositionClass::GetRotation(XMFLOAT3& p)
{
	p = m_rotation;
}

void PositionClass::MoveCameraPosition(float xMoveSpeed, float yMoveSpeed, float zMoveSpeed)
{
	//m_Camera->SetPosition(moveSpeed, 0.f, 0.f);

	XMFLOAT3 position = m_position;

	position.x += xMoveSpeed;
	position.y += yMoveSpeed;
	position.z += zMoveSpeed;

	m_position = position;

	//m_Camera->SetPosition(XMFLOAT3(position.x, position.y, position.z));
	//m_Light->SetPosition(position.x, position.y, position.z);

}

void PositionClass::RotateCameraPosition(float xRotataSpeed, float yRotataSpeed, float RotataSpeed)
{
	//m_Camera->SetPosition(moveSpeed, 0.f, 0.f);
	XMFLOAT3 position = m_rotation;

	position.x += xRotataSpeed;
	position.y += yRotataSpeed;
	position.z += RotataSpeed;

	m_rotation = position;
	//m_Camera->SetPosition(XMFLOAT3(0.5f, 2.f, -5.f));

}

//void PositionClass::xRotateMatrix(float angle)
//{
//	XMMATRIX worldMatrix;
//
//	m_Direct3D->GetWorldMatrix(worldMatrix);
//
//	worldMatrix = worldMatrix * XMMatrixRotationY(angle);
//
//	//m_Model->Render(m_Direct3D->GetDeviceContext());
//	m_Direct3D->SetWorldMatrix(worldMatrix);
//}


//void PositionClass::SetFrameTime(float time)
//{
//	m_frameTime = time;
//}


//void PositionClass::MoveForward(bool keydown)
//{
//	// 키를 누르면 카메라가 왼쪽으로 돌아는 속도가 증가합니다. 회전 속도를 늦추지 않으면.
//	if (keydown)
//	{
//		m_forwardSpeed += m_frameTime * 0.001f;
//
//		if (m_forwardSpeed > (m_frameTime * 0.03f))
//		{
//			m_forwardSpeed = m_frameTime * 0.03f;
//		}
//	}
//	else
//	{
//		m_forwardSpeed -= m_frameTime * 0.0007f;
//
//		if (m_forwardSpeed < 0.0f)
//		{
//			m_forwardSpeed = 0.0f;
//		}
//	}
//
//	// 값을 라디안으로 변환하십시오.
//	float radians = m_rotation.y * 0.0174532925f;
//
//	// 위치를 업데이트 합니다.
//	m_position.x += sinf(radians) * m_forwardSpeed;
//	m_position.z += cosf(radians) * m_forwardSpeed;
//}
//
//
//void PositionClass::MoveBackward(bool keydown)
//{
//	// 키를 누르면 카메라가 오른쪽으로 회전하는 속도가 증가합니다. 회전 속도를 늦추지 않으면.
//	if (keydown)
//	{
//		m_backwardSpeed += m_frameTime * 0.001f;
//
//		if (m_backwardSpeed > (m_frameTime * 0.03f))
//		{
//			m_backwardSpeed = m_frameTime * 0.03f;
//		}
//	}
//	else
//	{
//		m_backwardSpeed -= m_frameTime * 0.0007f;
//
//		if (m_backwardSpeed < 0.0f)
//		{
//			m_backwardSpeed = 0.0f;
//		}
//	}
//
//	// 값을 라디안으로 변환하십시오.
//	float radians = m_rotation.y * 0.0174532925f;
//
//	// 위치를 업데이트 합니다.
//	m_position.x -= sinf(radians) * m_backwardSpeed;
//	m_position.z -= cosf(radians) * m_backwardSpeed;
//}
//
//
//void PositionClass::MoveUpward(bool keydown)
//{
//	// 사용자가 키를 누르고 있는 프레임 시간에 따라 상향 이동 속도를 갱신합니다.
//	if (keydown)
//	{
//		m_upwardSpeed += m_frameTime * 0.003f;
//
//		if (m_upwardSpeed > (m_frameTime * 0.03f))
//		{
//			m_upwardSpeed = m_frameTime * 0.03f;
//		}
//	}
//	else
//	{
//		m_upwardSpeed -= m_frameTime * 0.002f;
//
//		if (m_upwardSpeed < 0.0f)
//		{
//			m_upwardSpeed = 0.0f;
//		}
//	}
//
//	// 높이 위치를 업데이트 합니다.
//	m_position.y += m_upwardSpeed;
//}
//
//
//void PositionClass::MoveDownward(bool keydown)
//{
//	// 사용자가 키를 누르고 있는 프레임 시간에 따라 하향 이동 속도를 갱신합니다.
//	if (keydown)
//	{
//		m_downwardSpeed += m_frameTime * 0.003f;
//
//		if (m_downwardSpeed > (m_frameTime * 0.03f))
//		{
//			m_downwardSpeed = m_frameTime * 0.03f;
//		}
//	}
//	else
//	{
//		m_downwardSpeed -= m_frameTime * 0.002f;
//
//		if (m_downwardSpeed < 0.0f)
//		{
//			m_downwardSpeed = 0.0f;
//		}
//	}
//
//	// 높이 위치를 업데이트 합니다.
//	m_position.y -= m_downwardSpeed;
//}
//
//
//void PositionClass::TurnLeft(bool keydown)
//{
//	// 사용자가 키를 누르고 있는 프레임 시간에 따라 왼쪽 회전 속도를 갱신합니다.
//	if (keydown)
//	{
//		m_leftTurnSpeed += m_frameTime * 0.01f;
//
//		if (m_leftTurnSpeed > (m_frameTime * 0.15f))
//		{
//			m_leftTurnSpeed = m_frameTime * 0.15f;
//		}
//	}
//	else
//	{
//		m_leftTurnSpeed -= m_frameTime * 0.005f;
//
//		if (m_leftTurnSpeed < 0.0f)
//		{
//			m_leftTurnSpeed = 0.0f;
//		}
//	}
//
//	// 회전을 업데이트 합니다.
//	m_rotation.y -= m_leftTurnSpeed;
//
//	// 회전값의 범위를 0 ~360 에서 유지합니다.
//	if (m_rotation.y < 0.0f)
//	{
//		m_rotation.y += 360.0f;
//	}
//}
//
//
//void PositionClass::TurnRight(bool keydown)
//{
//	// 사용자가 키를 누르고 있는 프레임 시간에 따라 오른쪽 회전 속도를 갱신합니다.
//	if (keydown)
//	{
//		m_rightTurnSpeed += m_frameTime * 0.01f;
//
//		if (m_rightTurnSpeed > (m_frameTime * 0.15f))
//		{
//			m_rightTurnSpeed = m_frameTime * 0.15f;
//		}
//	}
//	else
//	{
//		m_rightTurnSpeed -= m_frameTime * 0.005f;
//
//		if (m_rightTurnSpeed < 0.0f)
//		{
//			m_rightTurnSpeed = 0.0f;
//		}
//	}
//
//	// 회전을 업데이트 합니다.
//	m_rotation.y += m_rightTurnSpeed;
//
//	// 회전값의 범위를 0 ~360 에서 유지합니다.
//	if (m_rotation.y > 360.0f)
//	{
//		m_rotation.y -= 360.0f;
//	}
//}
//
//
//void PositionClass::LookUpward(bool keydown)
//{
//	// 사용자가 키를 누르고 있는 프레임 시간에 따라 위쪽 회전 속도를 갱신합니다.
//	if (keydown)
//	{
//		m_lookUpSpeed += m_frameTime * 0.01f;
//
//		if (m_lookUpSpeed > (m_frameTime * 0.15f))
//		{
//			m_lookUpSpeed = m_frameTime * 0.15f;
//		}
//	}
//	else
//	{
//		m_lookUpSpeed -= m_frameTime * 0.005f;
//
//		if (m_lookUpSpeed < 0.0f)
//		{
//			m_lookUpSpeed = 0.0f;
//		}
//	}
//
//	// 회전을 업데이트 합니다.
//	m_rotation.x -= m_lookUpSpeed;
//
//	// 회전은 최대 90 도로 유지 합니다.
//	if (m_rotation.x > 90.0f)
//	{
//		m_rotation.x = 90.0f;
//	}
//}
//
//
//void PositionClass::LookDownward(bool keydown)
//{
//	// 사용자가 키를 누르고 있는 프레임 시간에 따라 아래쪽 회전 속도를 갱신합니다.
//	if (keydown)
//	{
//		m_lookDownSpeed += m_frameTime * 0.01f;
//
//		if (m_lookDownSpeed > (m_frameTime * 0.15f))
//		{
//			m_lookDownSpeed = m_frameTime * 0.15f;
//		}
//	}
//	else
//	{
//		m_lookDownSpeed -= m_frameTime * 0.005f;
//
//		if (m_lookDownSpeed < 0.0f)
//		{
//			m_lookDownSpeed = 0.0f;
//		}
//	}
//
//	// 회전을 업데이트 합니다.
//	m_rotation.x += m_lookDownSpeed;
//
//	//회전은 최대 90 도로 유지 합니다.
//	if (m_rotation.x < -90.0f)
//	{
//		m_rotation.x = -90.0f;
//	}
//}