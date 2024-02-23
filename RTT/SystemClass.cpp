#include "stdafx.h"
#include "inputclass.h"
#include "graphicsclass.h"
#include "systemclass.h"


SystemClass::SystemClass()
{
}


SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}


bool SystemClass::Initialize()
{
	// ������ â ����, ���� ���� ���� �ʱ�ȭ
	int screenWidth = 0;
	int screenHeight = 0;

	// ������ ���� �ʱ�ȭ
	InitializeWindows(screenWidth, screenHeight);

	// m_Input ��ü ����. �� Ŭ������ ���� ������� Ű���� �Է� ó���� ���˴ϴ�.
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// m_Input ��ü �ʱ�ȭ
	if (!m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight))
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// m_Graphics ��ü ����.  �׷��� �������� ó���ϱ� ���� ��ü�Դϴ�.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	// m_Graphics ��ü �ʱ�ȭ.
	if (!m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd))
	{
		return false;
	}

	return true;
}


void SystemClass::Shutdown()
{
	// m_Graphics ��ü ��ȯ
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// m_Input ��ü ��ȯ
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// Window ���� ó��
	ShutdownWindows();
}


void SystemClass::Run()
{
	// �޽��� ����ü ���� �� �ʱ�ȭ
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// ����ڷκ��� ���� �޽����� ���������� �޽��������� ���ϴ�
	while (true)
	{
		// ������ �޽����� ó���մϴ�
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// ���� �޽����� ���� ��� �޽��� ������ Ż���մϴ�
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// �� �ܿ��� Frame �Լ��� ó���մϴ�.
			if (!Frame())
			{
				MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				break;
			}
		}

		// ����ڰ� ESCŰ�� �������� Ȯ�� �� ���� ó����
		if (m_Input->IsEscapePressed())
		{
			break;
		}

		
	}
}


bool SystemClass::Frame()
{
	// �Է� ������ ó���� �����մϴ�
	if (!m_Input->Frame())
	{
		return false;
	}
	if (m_Input->IsLeftArrowPressed() == true)
	{
		m_Graphics->MoveCameraPosition(0.1f, 0.f, 0.f);
	}

	if (m_Input->IsRightArrowPressed() == true)
	{
		m_Graphics->MoveCameraPosition(-0.1f, 0.f, 0.f);
	}

	if (m_Input->IsUpArrowPressed() == true)
	{
		m_Graphics->MoveCameraPosition(0.f, -0.1f, 0.f);
	}

	if (m_Input->IsDownArrowPressed() == true)
	{
		m_Graphics->MoveCameraPosition(0.f, 0.1f, 0.f);
	}

	if (m_Input->IsZPressed() == true)
	{
		m_Graphics->MoveCameraPosition(0.f, 0.f, 0.1f);
	}

	if (m_Input->IsXPressed() == true)
	{
		m_Graphics->MoveCameraPosition(0.f, 0.f, -0.1f);
	}

	if (m_Input->IsAPressed() == true)
	{
		m_Graphics->RotateCameraPosition(0.3f, 0.f, 0.f);
	}

	if (m_Input->IsDPressed() == true)
	{
		m_Graphics->RotateCameraPosition(-0.3f, 0.f, 0.f);
	}

	if (m_Input->IsIPressed() == true)
	{
		m_Graphics->xRotateMatrix(10.f);

	}
	// �׷��� ��ü�� ���� ������ ó���� �����մϴ�.
	if (!m_Graphics->Frame())
	{
		return false;
	}

	return m_Graphics->Render();
}

//bool SystemClass::Frame()
//{
//	//static float xMoveSpeed = 0.1f;
//	float xMoveSpeed = 0.1f;
//	float yMoveSpeed = 0.1f;
//	float zMoveSpeed = 0.1f;
//
//	float xRotataSpeed = 0.1f;
//	float yRotataSpeed = 0.1f;
//	float zRotataSpeed = 0.1f;
//
//	// ESC Ű ���� �� ���� ���θ� ó���մϴ�
//	if (m_Input->IsKeyDown(VK_ESCAPE))
//	{
//		return false;
//	}
//
//	else if (m_Input->IsKeyDown(VK_LEFT))
//	{
//
//		m_Graphics->MoveCameraPosition(xMoveSpeed, 0.f, 0.f);
//	}
//
//	else if (m_Input->IsKeyDown(VK_RIGHT))
//	{
//		m_Graphics->MoveCameraPosition(-xMoveSpeed, 0.f, 0.f);
//	}
//
//	else if (m_Input->IsKeyDown(VK_DOWN))
//	{
//
//		m_Graphics->MoveCameraPosition(0.f, yMoveSpeed, 0.f);
//	}
//
//	else if (m_Input->IsKeyDown(VK_UP))
//	{
//		m_Graphics->MoveCameraPosition(0.f, -yMoveSpeed, 0.f);
//	}
//
//	else if (m_Input->IsKeyDown('W'))
//	{
//		m_Graphics->RotateCameraPosition(xRotataSpeed, 0.f, 0.f);
//	}
//
//	else if (m_Input->IsKeyDown('S'))
//	{
//		m_Graphics->RotateCameraPosition(-xRotataSpeed, 0.f, 0.f);
//	}
//
//	else if (m_Input->IsKeyDown('A'))
//	{
//
//		m_Graphics->RotateCameraPosition(0.f, yRotataSpeed, 0.f);
//	}
//
//	else if (m_Input->IsKeyDown('D'))
//	{
//		m_Graphics->RotateCameraPosition(0.f, -yRotataSpeed, 0.f);
//	}
//
//	else if (m_Input->IsKeyDown('Q'))
//	{
//		m_Graphics->MoveCameraPosition(0.f, 0.f, zMoveSpeed);
//	}
//
//	else if (m_Input->IsKeyDown('E'))
//	{
//		m_Graphics->MoveCameraPosition(0.f, 0.f, -zMoveSpeed);
//	}
//
//	// �׷��� ��ü�� Frame�� ó���մϴ�
//	return m_Graphics->Frame();
//}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	// �ܺ� �����͸� �� ��ü�� �����մϴ�
	ApplicationHandle = this;

	// �� ���α׷��� �ν��Ͻ��� �����ɴϴ�
	m_hinstance = GetModuleHandle(NULL);

	// ���α׷� �̸��� �����մϴ�
	m_applicationName = L"RTT";

	// windows Ŭ������ �Ʒ��� ���� �����մϴ�.
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// windows class�� ����մϴ�
	RegisterClassEx(&wc);

	// ����� ȭ���� �ػ󵵸� �о�ɴϴ�
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	// FULL_SCREEN ���� ���� ���� ȭ���� �����մϴ�.
	if (FULL_SCREEN)
	{
		// Ǯ��ũ�� ���� �����ߴٸ� ����� ȭ�� �ػ󵵸� ����ũ�� �ػ󵵷� �����ϰ� ������ 32bit�� �����մϴ�.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Ǯ��ũ������ ���÷��� ������ �����մϴ�.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		// ������ ����� ��� 800 * 600 ũ�⸦ �����մϴ�.
		screenWidth = 800;
		screenHeight = 600;

		// ������ â�� ����, ������ �� ��� ������ �մϴ�.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// �����츦 �����ϰ� �ڵ��� ���մϴ�.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� �����մϴ�
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
}


void SystemClass::ShutdownWindows()
{
	// Ǯ��ũ�� ��忴�ٸ� ���÷��� ������ �ʱ�ȭ�մϴ�.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// â�� �����մϴ�
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ���α׷� �ν��Ͻ��� �����մϴ�
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// �ܺ������� ������ �ʱ�ȭ�մϴ�
	ApplicationHandle = NULL;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// ������ ���Ḧ Ȯ���մϴ�
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// �����찡 �������� Ȯ���մϴ�
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// �� ���� ��� �޽������� �ý��� Ŭ������ �޽��� ó���� �ѱ�ϴ�.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}