#pragma once

class GraphicsClass;

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	bool IsLeftArrowPressed();
	bool IsRightArrowPressed();
	bool IsUpArrowPressed();
	bool IsDownArrowPressed();
	bool IsZPressed();
	bool IsXPressed();
	bool IsAPressed();
	bool IsDPressed();
	bool IsIPressed();


	void GetMouseLocation(int&, int&);

	//void Initialize();
	//void KeyDown(unsigned int);
	//void KeyUp(unsigned int);
	//bool IsKeyDown(unsigned int);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput = nullptr;
	IDirectInputDevice8* m_keyboard = nullptr;
	IDirectInputDevice8* m_mouse = nullptr;

	unsigned char m_keyboardState[256] = { 0, };
	DIMOUSESTATE m_mouseState;

	bool m_keys[256];

	int m_screenWidth = 0;
	int m_screenHeight = 0;
	int m_mouseX = 0;
	int m_mouseY = 0;

	GraphicsClass* m_Graphics = nullptr;

};