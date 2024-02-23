#pragma once

class CameraClass;

class PositionClass
{
public:
	PositionClass();
	PositionClass(const PositionClass&);
	~PositionClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	void GetPosition(XMFLOAT3&);
	void GetRotation(XMFLOAT3&);

	//void SetFrameTime(float);

	//void MoveForward(bool);
	//void MoveBackward(bool);
	//void MoveUpward(bool);
	//void MoveDownward(bool);
	//void TurnLeft(bool);
	//void TurnRight(bool);
	//void LookUpward(bool);
	//void LookDownward(bool);

	void MoveCameraPosition(float xMoveSpeed, float yMoveSpeed, float zMoveSpeed);
	void RotateCameraPosition(float xRotataSpeed, float yRotataSpeed, float RotataSpeed);


private:
	XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float m_frameTime = 0;

	//float m_forwardSpeed = 0;
	//float m_backwardSpeed = 0;
	//float m_upwardSpeed = 0;
	//float m_downwardSpeed = 0;
	//float m_leftTurnSpeed = 0;
	//float m_rightTurnSpeed = 0;
	//float m_lookUpSpeed = 0;
	//float m_lookDownSpeed = 0;

	CameraClass* m_Camera = nullptr;
};