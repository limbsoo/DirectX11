#include "stdafx.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "ModelClass.h"
#include "lightshaderclass.h"
#include "lightclass.h"
#include "rendertextureclass.h"
#include "debugwindowclass.h"
#include "textureshaderclass.h"
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D ��ü ����
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	// Direct3D ��ü �ʱ�ȭ
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// m_Camera ��ü ����
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// �� ��ü ����
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}


	// ī�޶� ������ ����
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// m_Model ��ü �ʱ�ȭ
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../RTT/data/leather-texture-11614075174.tga", "../RTT/data/sphere.obj"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// ����Ʈ ���̴� ��ü�� ����ϴ�.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader)
	{
		return false;
	}

	// ����Ʈ ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if(!m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// ���� ��ü�� ����ϴ�.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// ���� ��ü�� �ʱ�ȭ�մϴ�.
	//m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, -5.0f, 5.0f);

	// ������ �ؽ�ó ��ü�� �����Ѵ�.
	m_RenderTexture = new RenderTextureClass;
	if(!m_RenderTexture)
	{
		return false;
	}

	// ������ �ؽ�ó ��ü�� �ʱ�ȭ�Ѵ�.
	if(!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
	{
		return false;
	}

	// ����� â ��ü�� ����ϴ�.
	m_DebugWindow = new DebugWindowClass;
	if(!m_DebugWindow)
	{
		return false;
	}

	// ����� â ��ü�� �ʱ�ȭ �մϴ�.
	if(!m_DebugWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, 100, 100))
	{
		MessageBox(hwnd, L"Could not initialize the debug window object.", L"Error", MB_OK);
		return false;
	}

	// �ؽ�ó ���̴� ��ü�� �����Ѵ�.
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader)
	{
		return false;
	}

	// �ؽ�ó ���̴� ��ü�� �ʱ�ȭ�Ѵ�.
	if(!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// �ؽ�ó ���̴� ��ü�� �����Ѵ�.
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// ����� â ��ü�� �����մϴ�.
	if (m_DebugWindow)
	{
		m_DebugWindow->Shutdown();
		delete m_DebugWindow;
		m_DebugWindow = 0;
	}

	// ������ �ؽ��� ��ü�� �������Ѵ�.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// ���� ��ü�� �����Ѵ�.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// ����Ʈ ���̴� ��ü�� �����մϴ�.
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// �� ��ü ��ȯ
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// m_Camera ��ü ��ȯ
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Direct3D ��ü ��ȯ
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}


bool GraphicsClass::Frame()
{
	//static float lightPositionX = -5.0f;

	//// �� �������� ���� ��ġ�� ������Ʈ�մϴ�.
	//lightPositionX += 0.05f;

	//if (lightPositionX > 5.0f)
	//{
	//	lightPositionX = -5.0f;
	//}

	//// ���� ��ġ�� ������Ʈ�մϴ�.
	//m_Light->SetPosition(lightPositionX, 8.0f, -5.0f);

	// �׷��� ����� ������Ʈ �մϴ�.

	return true;
}


bool GraphicsClass::Render()
{
	// ��ü ����� ���� �ؽ�ó�� �������մϴ�.
	if(!RenderToTexture())
	{
		return false;
	}

	// ���� �׸��� ���� ���۸� ����ϴ�
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// �� ������ ����� ���������� �������մϴ�.
	if(!RenderScene())
	{
		return false;
	}

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	//m_Direct3D->TurnZBufferOff();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	//����� ������ ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
	if(!m_DebugWindow->Render(m_Direct3D->GetDeviceContext(), 350, 250))
	{
		return false;
	}

	 //�ؽ�ó ���̴��� ����� ����� �����츦 �������Ѵ�.
	if(!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, viewMatrix, 
									 orthoMatrix, m_RenderTexture->GetShaderResourceView()))
	{
		return false;
	}

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	m_Direct3D->TurnZBufferOn();
	
	// ������ ������ ȭ�鿡 ����մϴ�
	m_Direct3D->EndScene();

	return true;
}


bool GraphicsClass::RenderToTexture()
{
	// ������ ����� �������� �°� �����մϴ�.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// �������� �ؽ�ó�� ����ϴ�.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

	// ���� ����� �������ϸ� �� ���� ��� �ؽ�ó�� �������˴ϴ�.

	if(!RenderTextureScene())
	{
		return false;
	}

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}


bool GraphicsClass::RenderScene()
{
	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// �� �������� rotation ������ ������Ʈ�մϴ�.
	static float rotation = 0.0f;
	rotation += (float)XM_PI * 0.0025f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// ȸ�� ������ ���� ����� ȸ���մϴ�.
	worldMatrix = XMMatrixRotationY(rotation);

	// �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������ �մϴ�.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// ����Ʈ ���̴��� ����Ͽ� ���� �������մϴ�.
	//return m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
	//							   m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	return m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(), m_Light->GetDiffuseColor());

}

bool GraphicsClass::RenderTextureScene()
{
	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// �� �������� rotation ������ ������Ʈ�մϴ�.
	static float rotation = 0.0f;
	rotation += (float)XM_PI * 0.0025f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// ȸ�� ������ ���� ����� ȸ���մϴ�.
	worldMatrix = XMMatrixRotationY(rotation);

	// �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������ �մϴ�.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//m_renderTextureView = m_Model->GetTexture();

	// ����Ʈ ���̴��� ����Ͽ� ���� �������մϴ�.
	return m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());

}

void GraphicsClass::MoveCameraPosition(float xMoveSpeed, float yMoveSpeed, float zMoveSpeed)
{
	//m_Camera->SetPosition(moveSpeed, 0.f, 0.f);
	XMFLOAT3 position = m_Camera->GetPosition();

	position.x += xMoveSpeed;
	position.y += yMoveSpeed;
	position.z += zMoveSpeed;

	m_Camera->SetPosition(position.x, position.y, position.z);

}

void GraphicsClass::RotateCameraPosition(float xRotataSpeed, float yRotataSpeed, float RotataSpeed)
{
	//m_Camera->SetPosition(moveSpeed, 0.f, 0.f);
	XMFLOAT3 position = m_Camera->GetRotation();

	position.x += xRotataSpeed;
	position.y += yRotataSpeed;
	position.z += RotataSpeed;

	m_Camera->SetRotation(position.x, position.y, position.z);

}

void GraphicsClass::xRotateMatrix(float angle)
{
	XMMATRIX worldMatrix;

	m_Direct3D->GetWorldMatrix(worldMatrix);

	worldMatrix = worldMatrix * XMMatrixRotationY(angle);

	//m_Model->Render(m_Direct3D->GetDeviceContext());
	m_Direct3D->SetWorldMatrix(worldMatrix);

}