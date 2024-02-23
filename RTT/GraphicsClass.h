#pragma once

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class D3DClass;
class CameraClass;
class ModelClass;
class LightShaderClass;
class LightClass;
class RenderTextureClass;
class DebugWindowClass;
class TextureShaderClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool Render();

	void MoveCameraPosition(float xMoveSpeed, float yMoveSpeed, float zMoveSpeed);
	void RotateCameraPosition(float xRotataSpeed, float yRotataSpeed, float zRotataSpeed);

	void xRotateMatrix(float angle);

private:
	bool RenderToTexture();
	bool RenderScene();
	bool RenderTextureScene();

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	ModelClass* m_Model = nullptr;
	ModelClass* m_Plane = nullptr;

	LightShaderClass* m_LightShader = nullptr;
	LightClass* m_Light = nullptr;
	RenderTextureClass* m_RenderTexture = nullptr;
	DebugWindowClass* m_DebugWindow = nullptr;
	TextureShaderClass* m_TextureShader = nullptr;

	ID3D11ShaderResourceView* m_renderTextureView = nullptr;
};