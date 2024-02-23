#include "stdafx.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "ModelClass.h"
#include "DepthShaderClass.h"
#include "lightclass.h"
#include "rendertextureclass.h"
#include "debugwindowclass.h"
#include "ShadowShaderClass.h"
#include "graphicsclass.h"

#include "TextureClass.h"
#include "TextureShaderClass.h"

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
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR))
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

	m_Camera->SetPosition(XMFLOAT3(1.8f, 3.5f, -10.0f));
	//m_Camera->SetPosition(XMFLOAT3(0.5f, 2.f, -5.f));

	// �� ��ü ����
	m_Sphere = new ModelClass;
	if (!m_Sphere)
	{
		return false;
	}

	// m_Model ��ü �ʱ�ȭ
	if (!m_Sphere->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../environmentMap/data/leather-texture-11614075174.tga", "../environmentMap/data/sphere.obj"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	//m_Sphere->SetPosition(0.0f, 1.0f, 0.0f);

	m_Plane = new ModelClass;
	if (!m_Plane)
	{
		return false;
	}

	// m_Model ��ü �ʱ�ȭ
	if (!m_Plane->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../environmentMap/data/silver-shimmery-paper-background.tga", "../environmentMap/data/plane.obj"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);         
		return false;
	}

	//m_Plane->SetPosition(0.0f, 1.0f, 0.0f);

	// ���� ��ü�� ����ϴ�.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// ���� ��ü�� �ʱ�ȭ�մϴ�.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->GenerateProjectionMatrix(SCREEN_NEAR, SCREEN_FAR);

	// ������ �ؽ�ó ��ü�� �����Ѵ�.
	m_RenderTexture = new RenderTextureClass;
	if(!m_RenderTexture)
	{
		return false;
	}

	// ������ �ؽ�ó ��ü�� �ʱ�ȭ�Ѵ�.

	if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_NEAR, SCREEN_FAR))
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

	// ����Ʈ ���̴� ��ü�� ����ϴ�.
	m_DepthShader = new DepthShaderClass;
	if (!m_DepthShader)
	{
		return false;
	}

	// ����Ʈ ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// �ؽ�ó ���̴� ��ü�� �����Ѵ�.
	m_ShadowShader = new ShadowShaderClass;
	if(!m_ShadowShader)
	{
		return false;
	}

	// �ؽ�ó ���̴� ��ü�� �ʱ�ȭ�Ѵ�.
	if(!m_ShadowShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_texture = new TextureClass;
	if (!m_texture)
	{
		return false;
	}

	if (!m_texture->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../environmentMap/data/spheremap1.tga"))
	{
		return false;
	}

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// �ؽ�ó ���̴� ��ü�� �ʱ�ȭ�Ѵ�.
	if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// �ؽ�ó ���̴� ��ü�� �����Ѵ�.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// �ؽ�ó ���̴� ��ü�� �����Ѵ�.
	if(m_ShadowShader)
	{
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
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
	if(m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	// �� ��ü ��ȯ
	if (m_Sphere)
	{
		m_Sphere->Shutdown();
		delete m_Sphere;
		m_Sphere = 0;
	}

	if (m_Plane)
	{
		m_Plane->Shutdown();
		delete m_Plane;
		m_Plane = 0;
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

	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}

	//if (m_TextureShader)
	//{
	//	m_TextureShader->Shutdown();
	//	delete m_TextureShader;
	//	m_TextureShader = 0;
	//}
}

bool GraphicsClass::Frame()
{
	static float lightPositionX = -5.0f;

	// �� �������� ���� ��ġ�� ������Ʈ�մϴ�.
	//lightPositionX += 0.05f;

	//if (lightPositionX > 5.0f)
	//{
	//	lightPositionX = -5.0f;
	//}

	// ���� ��ġ�� ������Ʈ�մϴ�.
	//m_Light->SetPosition(lightPositionX, 5.0f, -5.0f);
	m_Light->SetPosition(lightPositionX, 3.0f, -4.0f);

	// �׷��� ����� ������Ʈ �մϴ�.
	return Render();
}

void GraphicsClass::MoveCameraPosition(float xMoveSpeed, float yMoveSpeed, float zMoveSpeed)
{
	XMFLOAT3 position = m_Camera->GetPosition();

	position.x += xMoveSpeed;
	position.y += yMoveSpeed;
	position.z += zMoveSpeed;

	m_Camera->SetPosition(XMFLOAT3(position.x, position.y, position.z));
}

void GraphicsClass::RotateCameraPosition(float xRotataSpeed, float yRotataSpeed, float RotataSpeed)
{
	XMFLOAT3 position = m_Camera->GetRotation();

	position.x += xRotataSpeed;
	position.y += yRotataSpeed;
	position.z += RotataSpeed;

	m_Camera->SetRotation(XMFLOAT3(position.x, position.y, position.z));
}

void GraphicsClass::xRotateMatrix(float angle)
{
	XMMATRIX worldMatrix;

	m_Direct3D->GetWorldMatrix(worldMatrix);

	worldMatrix = worldMatrix * XMMatrixRotationY(angle);

	//m_Model->Render(m_Direct3D->GetDeviceContext());
	m_Direct3D->SetWorldMatrix(worldMatrix);

}

bool GraphicsClass::RenderToTexture()
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, orthoMatrix;

	float posX = 0;
	float posY = 0;
	float posZ = 0;

	// ������ ����� �������� �°� �����մϴ�.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//m_Camera->Render();

	// ������ ��ġ�� ���� ���� view ����� �����մϴ�.
	m_Light->GenerateViewMatrix();

	// d3d ��ü���� ���� ����� �����ɴϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	//// ����Ʈ ������Ʈ�κ��� ����Ʈ�� ��� ���� ����� �����ɴϴ�.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	//m_Direct3D->GetOrthoMatrix(orthoMatrix);


	// ���� ���̴��� ���� �������մϴ�.
	m_Sphere->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	//static float rotation = 0.0f;
	//rotation += (float)XM_PI * 0.0025f;

	// ȸ�� ������ ���� ����� ȸ���մϴ�.
	//worldMatrix = XMMatrixRotationY(rotation);

	// ť�� �𵨿� ���� ��ȯ ����� �����Ͻʽÿ�.


	m_Sphere->Render(m_Direct3D->GetDeviceContext());
	if (!m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Sphere->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix))
	{
		return false;
	}

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	m_Plane->Render(m_Direct3D->GetDeviceContext());
	if (!m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Plane->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix))
	{
		return false;
	}

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}


bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;

	// ���� ����� �ؽ�ó�� �������մϴ�.
	if(!RenderToTexture())
	{
		return false;
	}

	// ����� ������ ���۸� �����.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	// ������ ��ġ�� ���� ������ ����� �����մϴ�.
	m_Light->GenerateViewMatrix();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	//worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.2f, 0.2f, 0.2f))

	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	//worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.2f, 0.2f, 0.2f))

	//m_Direct3D->GetDeviceContext()->OMSetDepthStencilState(m_Direct3D->m_depthStencilState, 1);

	// �� ������ �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.1
	//m_Plane->Render(m_Direct3D->GetDeviceContext());

	////m_Direct3D->GetWorldMatrix(worldMatrix);

	//if (!m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Plane->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
	//	lightProjectionMatrix, m_Plane->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(), m_Light->GetAmbientColor(),
	//	m_Light->GetDiffuseColor(), m_texture->GetTexture(), m_Camera->GetPosition()))
	//{
	//	return false;
	//}

	//static float rotation = 0.0f;
	//rotation += (float)XM_PI * 0.0025f;

	// ȸ�� ������ ���� ����� ȸ���մϴ�.
	//worldMatrix = XMMatrixRotationY(rotation);

	// �� ������ �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
	m_Sphere->Render(m_Direct3D->GetDeviceContext());

	if (!m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Sphere->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_Sphere->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(), m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(), m_texture->GetTexture(), m_Camera->GetPosition()))
	{
		return false;
	}

	

	

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	//m_Direct3D->TurnZBufferOff();


	// ����� ������ ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
	if (!m_DebugWindow->Render(m_Direct3D->GetDeviceContext(), 50, 50))
	{
		return false;
	}

	 //�ؽ�ó ���̴��� ����� ����� �����츦 �������Ѵ�.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, viewMatrix,
		orthoMatrix, m_RenderTexture->GetShaderResourceView()))
	{
		return false;
	}


	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	//m_Direct3D->TurnZBufferOn();

	// ������ �� ����� ȭ�鿡 ǥ���մϴ�.
	m_Direct3D->EndScene();

	return true;
}

