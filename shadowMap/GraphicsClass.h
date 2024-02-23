#pragma once

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_FAR = 100.f;
const float SCREEN_NEAR = 1.5f;
const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;

class D3DClass;
class CameraClass;
class ModelClass;
class ShadowShaderClass;
class LightClass;
class RenderTextureClass;
class DebugWindowClass;
class DepthShaderClass;

class TextureClass;
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
	bool RenderToTexture();

	void MoveCameraPosition(float xMoveSpeed, float yMoveSpeed, float zMoveSpeed);
	void RotateCameraPosition(float xRotataSpeed, float yRotataSpeed, float zRotataSpeed);
	void xRotateMatrix(float angle);

	XMFLOAT3 cameraPosition;
	XMFLOAT3 lightPosition;

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	ModelClass* m_Sphere = nullptr;
	ModelClass* m_Plane = nullptr;
	DepthShaderClass* m_DepthShader = nullptr;
	LightClass* m_Light = nullptr;
	RenderTextureClass* m_RenderTexture = nullptr;
	DebugWindowClass* m_DebugWindow = nullptr;
	ShadowShaderClass* m_ShadowShader = nullptr;
	ID3D11ShaderResourceView* m_renderTextureView = nullptr;

	TextureClass* m_texture = nullptr;
	TextureShaderClass* m_TextureShader = nullptr;
};