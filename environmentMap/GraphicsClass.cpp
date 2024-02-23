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
	// Direct3D 객체 생성
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	// Direct3D 객체 초기화
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// m_Camera 객체 생성
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	m_Camera->SetPosition(XMFLOAT3(1.8f, 3.5f, -10.0f));
	//m_Camera->SetPosition(XMFLOAT3(0.5f, 2.f, -5.f));

	// 모델 객체 생성
	m_Sphere = new ModelClass;
	if (!m_Sphere)
	{
		return false;
	}

	// m_Model 객체 초기화
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

	// m_Model 객체 초기화
	if (!m_Plane->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../environmentMap/data/silver-shimmery-paper-background.tga", "../environmentMap/data/plane.obj"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);         
		return false;
	}

	//m_Plane->SetPosition(0.0f, 1.0f, 0.0f);

	// 조명 객체를 만듭니다.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// 조명 객체를 초기화합니다.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->GenerateProjectionMatrix(SCREEN_NEAR, SCREEN_FAR);

	// 렌더링 텍스처 객체를 생성한다.
	m_RenderTexture = new RenderTextureClass;
	if(!m_RenderTexture)
	{
		return false;
	}

	// 렌더링 텍스처 객체를 초기화한다.

	if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_NEAR, SCREEN_FAR))
	{
		return false;
	}

	// 디버그 창 객체를 만듭니다.
	m_DebugWindow = new DebugWindowClass;
	if(!m_DebugWindow)
	{
		return false;
	}

	// 디버그 창 객체를 초기화 합니다.
	if(!m_DebugWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, 100, 100))
	{
		MessageBox(hwnd, L"Could not initialize the debug window object.", L"Error", MB_OK);
		return false;
	}

	// 라이트 쉐이더 객체를 만듭니다.
	m_DepthShader = new DepthShaderClass;
	if (!m_DepthShader)
	{
		return false;
	}

	// 라이트 쉐이더 객체를 초기화합니다.
	if (!m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// 텍스처 쉐이더 객체를 생성한다.
	m_ShadowShader = new ShadowShaderClass;
	if(!m_ShadowShader)
	{
		return false;
	}

	// 텍스처 쉐이더 객체를 초기화한다.
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

	// 텍스처 쉐이더 객체를 초기화한다.
	if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// 텍스처 쉐이더 객체를 해제한다.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// 텍스처 쉐이더 객체를 해제한다.
	if(m_ShadowShader)
	{
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	}

	// 디버그 창 객체를 해제합니다.
	if (m_DebugWindow)
	{
		m_DebugWindow->Shutdown();
		delete m_DebugWindow;
		m_DebugWindow = 0;
	}

	// 렌더를 텍스쳐 객체로 릴리즈한다.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// 조명 객체를 해제한다.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// 라이트 쉐이더 객체를 해제합니다.
	if(m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	// 모델 객체 반환
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

	// m_Camera 객체 반환
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Direct3D 객체 반환
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

	// 각 프레임의 조명 위치를 업데이트합니다.
	//lightPositionX += 0.05f;

	//if (lightPositionX > 5.0f)
	//{
	//	lightPositionX = -5.0f;
	//}

	// 빛의 위치를 업데이트합니다.
	//m_Light->SetPosition(lightPositionX, 5.0f, -5.0f);
	m_Light->SetPosition(lightPositionX, 3.0f, -4.0f);

	// 그래픽 장면을 업데이트 합니다.
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

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//m_Camera->Render();

	// 조명의 위치에 따라 조명 view 행렬을 생성합니다.
	m_Light->GenerateViewMatrix();

	// d3d 객체에서 세계 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	//// 라이트 오브젝트로부터 라이트의 뷰와 투영 행렬을 가져옵니다.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	//m_Direct3D->GetOrthoMatrix(orthoMatrix);


	// 깊이 셰이더로 모델을 렌더링합니다.
	m_Sphere->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	//static float rotation = 0.0f;
	//rotation += (float)XM_PI * 0.0025f;

	// 회전 값으로 월드 행렬을 회전합니다.
	//worldMatrix = XMMatrixRotationY(rotation);

	// 큐브 모델에 대한 변환 행렬을 설정하십시오.


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

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}


bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;

	// 먼저 장면을 텍스처로 렌더링합니다.
	if(!RenderToTexture())
	{
		return false;
	}

	// 장면을 시작할 버퍼를 지운다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_Light->GenerateViewMatrix();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	//worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.2f, 0.2f, 0.2f))

	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	//worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.2f, 0.2f, 0.2f))

	//m_Direct3D->GetDeviceContext()->OMSetDepthStencilState(m_Direct3D->m_depthStencilState, 1);

	// 모델 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.1
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

	// 회전 값으로 월드 행렬을 회전합니다.
	//worldMatrix = XMMatrixRotationY(rotation);

	// 모델 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	m_Sphere->Render(m_Direct3D->GetDeviceContext());

	if (!m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Sphere->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_Sphere->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(), m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(), m_texture->GetTexture(), m_Camera->GetPosition()))
	{
		return false;
	}

	

	

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	//m_Direct3D->TurnZBufferOff();


	// 디버그 윈도우 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	if (!m_DebugWindow->Render(m_Direct3D->GetDeviceContext(), 50, 50))
	{
		return false;
	}

	 //텍스처 셰이더를 사용해 디버그 윈도우를 렌더링한다.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, viewMatrix,
		orthoMatrix, m_RenderTexture->GetShaderResourceView()))
	{
		return false;
	}


	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	//m_Direct3D->TurnZBufferOn();

	// 렌더링 된 장면을 화면에 표시합니다.
	m_Direct3D->EndScene();

	return true;
}

