//--------------------------------------------------------------------------------------
// File: Effects_In_DX11.cpp
// Made By zhangbo0037, Soongsil University.
// Email:zhangbo0037@qq.com             (2016.10.15)
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "DXUTCamera.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "resource.h"
#include "Animation.h"
#include "LightHelper.h"
#include "RenderHelper.h"
#include "rendertextureclass.h"
#include <atlstr.h> 
#include <string>
//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
# pragma region variables

CFirstPersonCamera                      g_Camera;                // The first person viewer camera          
ID3D10Blob* effectBlob = NULL;
ID3D10Blob* errorsBlob = NULL;
CDXUTDialogResourceManager              g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg                         g_SettingsDlg;          // Device settings dialog
CDXUTTextHelper* g_pTxtHelper = NULL;
CDXUTDialog                             g_HUD;                  // dialog for standard controls
CDXUTDialog                             g_SampleUI;             // dialog for sample specific controls
ID3D11InputLayout* g_pLayout = NULL;
ID3D11SamplerState* g_pSamLinear = NULL;

ID3D11Buffer* g_pcbVSPerObject_1 = NULL;
ID3D11Buffer* g_pcbVSPerObject_2 = NULL;
ID3D11Buffer* g_pcbVSPerObject_3 = NULL;
ID3D11Buffer* g_pcbVSPerObjectPlane1 = NULL;

ID3D11Buffer* g_pcbVSPerFrame_1 = NULL;
ID3D11Buffer* g_pcbVSPerFrame_2 = NULL;
ID3D11Buffer* g_pcbVSPerFrame_3 = NULL;
ID3D11Buffer* g_pcbVSPerFramePlane1 = NULL;

ID3D11ShaderResourceView* g_pTextures_1 = NULL;
ID3D11ShaderResourceView* g_pTextures_2 = NULL;
ID3D11ShaderResourceView* g_pTextures_3 = NULL;
ID3D11ShaderResourceView* g_pTexturesPlane1 = NULL;

Animation* g_pModel_1 = NULL;
Animation* g_pModel_2 = NULL;
Animation* g_pModel_3 = NULL;
Animation* g_pModelPlane1 = NULL;

RenderTextureClass* g_color_Map_RenderClass[7];
RenderTextureClass* g_motionVector3D_Map_RenderClass[7];
RenderTextureClass* g_motionVector3DSquare_Map_RenderClass[7];
RenderTextureClass* g_previousDepth_Map_RenderClass[7];

//fullscreen
RenderTextureClass* g_fullScreenQuad_Map_RenderClass;
RenderTextureClass* g_fullScreenQuadDenoising_Map_RenderClass;

D3DXMATRIX g_mPreProjectionMatrix1;
D3DXMATRIX g_mPreProjectionMatrix2;
D3DXMATRIX g_mPreProjectionMatrix3;

D3DXMATRIX g_mPreProjectionMatrix7;
RenderTextureClass* g_MSAARenderTextureClass;

struct CameraBufferType {
	D3DXVECTOR3 cameraPosition;
	float padding;
};

struct LightBufferType {
	D3DXVECTOR4 ambientColor;
	D3DXVECTOR3 diffuseColor;
	D3DXVECTOR3 lightDirection;
	float specularPower;
	D3DXVECTOR4 specularColor;
};

# pragma endregion

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
# pragma region Forward declarations
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext);
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo* AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo* DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext);
void CALLBACK OnD3D11DestroyDevice(void* pUserContext);
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext);
void Rendering(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext);
//void ScreenShot(ID3D11DeviceContext* pd3dImmediateContext, IDXGISwapChain* pSwapChain,HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitApp();
void LoadModels(ID3D11Device* pd3dDevice);
# pragma endregion

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	// Set callbacks
	DXUTSetCallbackMsgProc(MsgProc);
	DXUTSetCallbackKeyboard(OnKeyboard);

	//ScreenShot(ID3D11DeviceContext * pd3dImmediateContext, IDXGISwapChain * pSwapChain,HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	DXUTSetCallbackFrameMove(OnFrameMove);
	DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
	DXUTSetCallbackD3D11DeviceAcceptable(IsD3D11DeviceAcceptable);
	DXUTSetCallbackD3D11DeviceCreated(OnD3D11CreateDevice);
	DXUTSetCallbackD3D11SwapChainResized(OnD3D11ResizedSwapChain);
	DXUTSetCallbackD3D11SwapChainReleasing(OnD3D11ReleasingSwapChain);
	DXUTSetCallbackD3D11DeviceDestroyed(OnD3D11DestroyDevice);
	DXUTSetCallbackD3D11FrameRender(OnD3D11FrameRender);
	InitApp();

	// Perform any application-level initialization here
	DXUTInit(true, true, NULL); // Parse the command line, show msgboxes on error, no extra command line params
	DXUTSetCursorSettings(true, true); // Show the cursor and clip it when in full screen
	DXUTCreateWindow(L"'Load & Motion & Texture & Lighting & Shadow Map");
	// Only require 10-level hardware, change to D3D_FEATURE_LEVEL_11_0 to require 11-class hardware
	// Switch to D3D_FEATURE_LEVEL_9_x for 10level9 hardware
	DXUTCreateDevice(D3D_FEATURE_LEVEL_11_0, true, screenWidth, screenHeight);
	DXUTMainLoop(); // Enter into the DXUT render loop
	return DXUTGetExitCode();
}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
	int iY = 30;
	int iYo = 26;
	g_SettingsDlg.Init(&g_DialogResourceManager);
	g_HUD.Init(&g_DialogResourceManager);
	g_HUD.SetCallback(OnGUIEvent);
	//g_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 170, 22);
	//g_HUD.AddButton(IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += iYo, 170, 22, VK_F3);
	//g_HUD.AddButton(IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += iYo, 170, 22, VK_F2);
	g_SampleUI.Init(&g_DialogResourceManager);
	g_SampleUI.SetCallback(OnGUIEvent);
	g_Camera.SetRotateButtons(true, false, false);
}

//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
	g_pTxtHelper->Begin();
	g_pTxtHelper->SetInsertionPos(5, 5);
	g_pTxtHelper->SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	g_pTxtHelper->DrawTextLine(DXUTGetFrameStats(DXUTIsVsyncEnabled()));
	g_pTxtHelper->DrawTextLine(DXUTGetDeviceStats());
	g_pTxtHelper->End();
}

//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo* AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo* DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	return true;
}

# pragma region ScreenShot and frame check
void ScreenShot() {


	static DWORD dwTimefirst = GetTickCount();
	if (GetTickCount() - dwTimefirst > 5000)
	{
		OutputDebugString(DXUTGetFrameStats(DXUTIsVsyncEnabled()));
		OutputDebugString(L"\n");
		dwTimefirst = GetTickCount();
	}
}
void checkGPUFrame(float fElapsedTime) {

	DXUT_BeginPerfEvent(DXUT_PERFEVENTCOLOR, L"HUD / Stats");
	g_HUD.OnRender(fElapsedTime);
	g_SampleUI.OnRender(fElapsedTime);
	RenderText();
	DXUT_EndPerfEvent();
}

# pragma endregion

# pragma region Load Models and texture
void LoadModels(ID3D11Device* pd3dDevice)
{
	g_pModel_1 = new Animation();
	g_pModel_2 = new Animation();
	g_pModel_3 = new Animation();
	g_pModelPlane1 = new Animation();

	g_pModel_1->LoadMesh("./models/Sphere/sphere.x", pd3dDevice);
	g_pModel_2->LoadMesh("./models/sphere/sphere.x", pd3dDevice);
	g_pModel_3->LoadMesh("./models/sphere/sphere.x", pd3dDevice);

	g_pModelPlane1->LoadMesh("./Models/table.obj", pd3dDevice);//billiard
}

HRESULT LoadTextures(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
	HRESULT hr;

	V_RETURN(D3DX11CreateShaderResourceViewFromFile(pd3dDevice, L"./models/Sphere/pbcue.jpg", 0, 0, &g_pTextures_1, 0));
	V_RETURN(D3DX11CreateShaderResourceViewFromFile(pd3dDevice, L"./Models/Sphere/billiard13.jpg", 0, 0, &g_pTextures_2, 0));
	V_RETURN(D3DX11CreateShaderResourceViewFromFile(pd3dDevice, L"./Models/Sphere/billiard14.jpg", 0, 0, &g_pTextures_3, 0));

	V_RETURN(D3DX11CreateShaderResourceViewFromFile(pd3dDevice, L"./Models/bilispoyta2.png", 0, 0, &g_pTexturesPlane1, 0));
}
# pragma endregion

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;
	ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
	V_RETURN(g_DialogResourceManager.OnD3D11CreateDevice(pd3dDevice, pd3dImmediateContext));
	V_RETURN(g_SettingsDlg.OnD3D11CreateDevice(pd3dDevice));
	g_pTxtHelper = new CDXUTTextHelper(pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15);

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	hr = D3DX11CompileFromFile(L"shader.fx", 0, 0, 0, "fx_5_0", dwShaderFlags, 0, 0, &effectBlob, &errorsBlob, 0);

	assert(SUCCEEDED(hr) && effectBlob);

	if (errorsBlob)
	{
		MessageBoxA(0, (char*)errorsBlob->GetBufferPointer(), 0, 0);
		errorsBlob->Release();
	}

	hr = D3DX11CreateEffectFromMemory(effectBlob->GetBufferPointer(), effectBlob->GetBufferSize(), 0, pd3dDevice, &g_pEffect);
	assert(SUCCEEDED(hr));
	effectBlob->Release();

	D3DX11_PASS_DESC passDesc;
	g_pTech = g_pEffect->GetTechniqueByName("RenderScene_Previous");
	g_pTech->GetPassByIndex(0)->GetDesc(&passDesc);

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	V_RETURN(pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &g_pLayout));

	///////////////////////////////////////////////////////////////////////////////////
	// Create state objects
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory(&samDesc, sizeof(samDesc));
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;
	V_RETURN(pd3dDevice->CreateSamplerState(&samDesc, &g_pSamLinear));
	DXUT_SetDebugName(g_pSamLinear, "Linear");

	// Create constant buffers of Main Object 1
	D3D11_BUFFER_DESC cbDesc_1;
	ZeroMemory(&cbDesc_1, sizeof(cbDesc_1));
	cbDesc_1.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc_1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc_1.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc_1.ByteWidth = sizeof(CB_VS_PER_OBJECT);
	V_RETURN(pd3dDevice->CreateBuffer(&cbDesc_1, NULL, &g_pcbVSPerObject_1));
	DXUT_SetDebugName(g_pcbVSPerObject_1, "CB_VS_PER_OBJECT");
	V_RETURN(pd3dDevice->CreateBuffer(&cbDesc_1, NULL, &g_pcbVSPerFrame_1));
	DXUT_SetDebugName(g_pcbVSPerFrame_1, "CB_VS_PER_FRAME");

	// Create constant buffers of Main Object 2
	D3D11_BUFFER_DESC cbDesc_2;
	ZeroMemory(&cbDesc_2, sizeof(cbDesc_2));
	cbDesc_2.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc_2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc_2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc_2.ByteWidth = sizeof(CB_VS_PER_OBJECT);
	V_RETURN(pd3dDevice->CreateBuffer(&cbDesc_2, NULL, &g_pcbVSPerObject_2));
	DXUT_SetDebugName(g_pcbVSPerObject_2, "CB_VS_PER_OBJECT");
	V_RETURN(pd3dDevice->CreateBuffer(&cbDesc_2, NULL, &g_pcbVSPerFrame_2));
	DXUT_SetDebugName(g_pcbVSPerFrame_2, "CB_VS_PER_FRAME");

	// Create constant buffers of Main Object 3
	D3D11_BUFFER_DESC cbDesc_3;
	ZeroMemory(&cbDesc_3, sizeof(cbDesc_3));
	cbDesc_3.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc_3.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc_3.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc_3.ByteWidth = sizeof(CB_VS_PER_OBJECT);
	V_RETURN(pd3dDevice->CreateBuffer(&cbDesc_3, NULL, &g_pcbVSPerObject_3));
	DXUT_SetDebugName(g_pcbVSPerObject_3, "CB_VS_PER_OBJECT");
	V_RETURN(pd3dDevice->CreateBuffer(&cbDesc_3, NULL, &g_pcbVSPerFrame_3));
	DXUT_SetDebugName(g_pcbVSPerFrame_3, "CB_VS_PER_FRAME");

	// Create constant buffers of Plane_1
	D3D11_BUFFER_DESC cbDescPlane1;
	ZeroMemory(&cbDescPlane1, sizeof(cbDescPlane1));
	cbDescPlane1.Usage = D3D11_USAGE_DYNAMIC;
	cbDescPlane1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDescPlane1.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDescPlane1.ByteWidth = sizeof(CB_VS_PER_OBJECT);
	V_RETURN(pd3dDevice->CreateBuffer(&cbDescPlane1, NULL, &g_pcbVSPerObjectPlane1));
	DXUT_SetDebugName(g_pcbVSPerObjectPlane1, "CB_VS_PER_OBJECT_PLANE_1");
	V_RETURN(pd3dDevice->CreateBuffer(&cbDescPlane1, NULL, &g_pcbVSPerFramePlane1));
	DXUT_SetDebugName(g_pcbVSPerFramePlane1, "CB_VS_PER_FRAME_PLANE_1");

	//----------------------------------------------------------------------------------
	LoadModels(pd3dDevice);
	LoadTextures(pd3dDevice, pd3dImmediateContext);
	//----------------------------------------------------------------------------------
	for (int i = 0; i < 7; i++) {
		g_color_Map_RenderClass[i] = new RenderTextureClass;//  initialize -> send it to rendertextureclass
		g_color_Map_RenderClass[i]->InitializeBackBufferWithMipMap(pd3dDevice, screenWidth, screenHeight);

		g_motionVector3D_Map_RenderClass[i] = new RenderTextureClass;
		g_motionVector3D_Map_RenderClass[i]->InitializeBackBufferWithMipMap(pd3dDevice, screenWidth, screenHeight);

		g_motionVector3DSquare_Map_RenderClass[i] = new RenderTextureClass;
		g_motionVector3DSquare_Map_RenderClass[i]->InitializeBackBufferWithMipMap(pd3dDevice, screenWidth, screenHeight);

		g_previousDepth_Map_RenderClass[i] = new RenderTextureClass;
		g_previousDepth_Map_RenderClass[i]->Initialize(pd3dDevice, screenWidth, screenHeight);
	}

	//fullscreen
	g_fullScreenQuad_Map_RenderClass = new RenderTextureClass;
	g_fullScreenQuad_Map_RenderClass->Initialize(pd3dDevice, pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);

	//Denoising
	g_fullScreenQuadDenoising_Map_RenderClass = new RenderTextureClass;
	g_fullScreenQuadDenoising_Map_RenderClass->Initialize(pd3dDevice, pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);

	g_MSAARenderTextureClass = new RenderTextureClass;
	g_MSAARenderTextureClass->SetMSAAInitialize(pd3dDevice, pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);

	// Eye of mian Camera
	D3DXVECTOR3 vecEye;
	D3DXVECTOR3 vecAt(-20.0f, 30.f, -20.0f);
	float scale = 0.06;
	vecEye = vecAt + D3DXVECTOR3(50, 100, 300) * scale;
	g_Camera.SetViewParams(&vecEye, &vecAt);

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;
	V_RETURN(g_DialogResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));
	V_RETURN(g_SettingsDlg.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));
	// Setup the camera's projection parameters
	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams(D3DX_PI / 4, fAspectRatio, 0.1f, 1000.0f);
	g_HUD.SetLocation(pBackBufferSurfaceDesc->Width - 170, 0);
	g_HUD.SetSize(170, 170);
	g_SampleUI.SetLocation(pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300);
	g_SampleUI.SetSize(170, 300);
	return S_OK;
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext)
{
	static int frame = 6;
	if (g_SettingsDlg.IsActive())
	{
		g_SettingsDlg.OnRender(fElapsedTime);
		return;
	}

	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Clear render target view
	const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
	// Clear depth stencil
	ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

	D3D11_VIEWPORT mViewport;
	ZeroMemory(&mViewport, sizeof(mViewport));
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = screenWidth;
	mViewport.Height = screenHeight;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	pd3dImmediateContext->RSSetViewports(1, &mViewport);

	///////////////////////////////test/////////////////////////////////////

	//ID3D11DepthStencilView* pDepthSV = g_MSAARenderTextureClass->GetDepthStencilView();
	//g_MSAARenderTextureClass->Clear(pd3dImmediateContext, pDepthSV, 0.0f, 0.0f, 0.0f, 1.0f);

	//ID3D11RenderTargetView* pRenderTarget = g_MSAARenderTextureClass->GetRenderTargetView();
	////pd3dImmediateContext->OMSetDepthStencilState(g_RenderTextureClass->GetWriteDepthStencilState(), 0);
	//pd3dImmediateContext->RSSetState(g_MSAARenderTextureClass->GetRasterizerState());
	//pd3dImmediateContext->OMSetRenderTargets(1, &pRenderTarget, pDepthSV);
	//
	//g_pTech = g_pEffect->GetTechniqueByName("firstScene");
	//Rendering(pd3dDevice, pd3dImmediateContext, 0, fElapsedTime, pUserContext);

	//for (int i = 0; i < KBUFFER_PASSES; i++) {
	//	g_MSAAScene[i]->SetResource(g_MSAARenderTextureClass->GetShaderResourceView());
	//}
	/////////////////////////////////////////////////////////////////////////
	//
	//g_fullScreenQuad_Map_RenderClass->Clear(pd3dImmediateContext, pDSV, 0.0f, 0.0f, 0.0f, 1.0f);
	//pd3dImmediateContext->OMSetRenderTargets(1, &pRTV, pDSV);

	//g_pTech = g_pEffect->GetTechniqueByName("MSAAScene");
	//g_pTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
	//pd3dImmediateContext->Draw(3, 0);
	// 
	//---------------------------------------------------------------------------------
	//		PASS 1 previous frame computing , motion vector map , motion vector square map ,previous depth map
	//---------------------------------------------------------------------------------

	ID3D11DepthStencilView* depthSV = g_color_Map_RenderClass[frame % 7]->GetDepthStencilView();

	g_color_Map_RenderClass[frame % 7]->Clear(pd3dImmediateContext, depthSV, 0.0f, 0.0f, 0.0f, 1.0f);
	g_motionVector3D_Map_RenderClass[frame % 7]->Clear(pd3dImmediateContext, depthSV, 0.0f, 0.0f, 0.0f, 1.0f);
	g_motionVector3DSquare_Map_RenderClass[frame % 7]->Clear(pd3dImmediateContext, depthSV, 0.0f, 0.0f, 0.0f, 1.0f);
	g_previousDepth_Map_RenderClass[frame % 7]->Clear(pd3dImmediateContext, depthSV, 0.0f, 0.0f, 0.0f, 1.0f);

	ID3D11RenderTargetView* renderTarget[] = {
		g_color_Map_RenderClass[frame % 7]->GetRenderTargetView(),
		g_motionVector3D_Map_RenderClass[frame % 7]->GetRenderTargetView(),
		g_motionVector3DSquare_Map_RenderClass[frame % 7]->GetRenderTargetView(),
		g_previousDepth_Map_RenderClass[frame % 7]->GetRenderTargetView()
	};
	pd3dImmediateContext->OMSetRenderTargets(4, renderTarget, depthSV);

	g_pTech = g_pEffect->GetTechniqueByName("RenderScene_Previous"); //VS, PS È£Ãâ
	Rendering(pd3dDevice, pd3dImmediateContext, 0, fElapsedTime, pUserContext);
	pd3dImmediateContext->OMGetRenderTargets(0, NULL, NULL);

	//---------------------------------------------------------------------------------
	//PASS 2 motion blur effect
	//---------------------------------------------------------------------------------
	pd3dImmediateContext->GenerateMips(g_color_Map_RenderClass[frame % 7]->GetShaderResourceView());
	pd3dImmediateContext->GenerateMips(g_motionVector3D_Map_RenderClass[frame % 7]->GetShaderResourceView());
	pd3dImmediateContext->GenerateMips(g_motionVector3DSquare_Map_RenderClass[frame % 7]->GetShaderResourceView());

	g_colorMap->SetResource(g_color_Map_RenderClass[frame % 7]->GetShaderResourceView());
	g_motionVector3DMap->SetResource(g_motionVector3D_Map_RenderClass[frame % 7]->GetShaderResourceView());
	g_previousDepthMap->SetResource(g_previousDepth_Map_RenderClass[frame % 7]->GetShaderResourceView());

	g_colorMap1->SetResource(g_color_Map_RenderClass[(frame - 1) % 7]->GetShaderResourceView());
	g_motionVector3DMap1->SetResource(g_motionVector3D_Map_RenderClass[(frame - 1) % 7]->GetShaderResourceView());
	g_previousDepthMap1->SetResource(g_previousDepth_Map_RenderClass[(frame - 1) % 7]->GetShaderResourceView());

	g_colorMap2->SetResource(g_color_Map_RenderClass[(frame - 2) % 7]->GetShaderResourceView());
	g_motionVector3DMap2->SetResource(g_motionVector3D_Map_RenderClass[(frame - 2) % 7]->GetShaderResourceView());
	g_previousDepthMap2->SetResource(g_previousDepth_Map_RenderClass[(frame - 2) % 7]->GetShaderResourceView());

	g_colorMap3->SetResource(g_color_Map_RenderClass[(frame - 3) % 7]->GetShaderResourceView());
	g_motionVector3DMap3->SetResource(g_motionVector3D_Map_RenderClass[(frame - 3) % 7]->GetShaderResourceView());
	g_previousDepthMap3->SetResource(g_previousDepth_Map_RenderClass[(frame - 3) % 7]->GetShaderResourceView());

	g_colorMap4->SetResource(g_color_Map_RenderClass[(frame - 4) % 7]->GetShaderResourceView());
	g_motionVector3DMap4->SetResource(g_motionVector3D_Map_RenderClass[(frame - 4) % 7]->GetShaderResourceView());
	g_previousDepthMap4->SetResource(g_previousDepth_Map_RenderClass[(frame - 4) % 7]->GetShaderResourceView());

	g_colorMap5->SetResource(g_color_Map_RenderClass[(frame - 5) % 7]->GetShaderResourceView());
	g_motionVector3DMap5->SetResource(g_motionVector3D_Map_RenderClass[(frame - 5) % 7]->GetShaderResourceView());
	g_previousDepthMap5->SetResource(g_previousDepth_Map_RenderClass[(frame - 5) % 7]->GetShaderResourceView());

	g_colorMap6->SetResource(g_color_Map_RenderClass[(frame - 6) % 7]->GetShaderResourceView());
	g_motionVector3DMap6->SetResource(g_motionVector3D_Map_RenderClass[(frame - 6) % 7]->GetShaderResourceView());
	g_previousDepthMap6->SetResource(g_previousDepth_Map_RenderClass[(frame - 6) % 7]->GetShaderResourceView());

	g_motionVector3DSquareMap->SetResource(g_motionVector3DSquare_Map_RenderClass[frame % 7]->GetShaderResourceView());
	frame++;

	g_fullScreenQuad_Map_RenderClass->SetRenderTarget(pd3dImmediateContext, pDSV);
	g_fullScreenQuad_Map_RenderClass->Clear(pd3dImmediateContext, pDSV, 0.0f, 0.0f, 0.0f, 1.0f);
	g_pTech = g_pEffect->GetTechniqueByName("RenderScene_FullScreenQuad");

	g_pTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
	pd3dImmediateContext->Draw(3, 0);
	pd3dImmediateContext->OMGetRenderTargets(0, NULL, NULL);

	g_FullscreenQuad->SetResource(g_fullScreenQuad_Map_RenderClass->GetShaderResourceView());

	//---------------------------------------------------------------------------------
	//PASS 3 Denoising
	//---------------------------------------------------------------------------------

	pd3dImmediateContext->ClearRenderTargetView(pRTV, ClearColor);
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0F, 0);
	pd3dImmediateContext->OMSetRenderTargets(1, &pRTV, pDSV);// 
	g_pTech = g_pEffect->GetTechniqueByName("RenderScene_FullScreenQuad_Denoising");//
	g_pTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
	pd3dImmediateContext->Draw(3, 0);
	pd3dImmediateContext->OMGetRenderTargets(0, NULL, NULL);

	////ScreenShot();
	checkGPUFrame(fElapsedTime);
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void Rendering(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext)
{
	// Prepare for Motion of object
	GetEffect();
	D3DXMATRIX mView = *g_Camera.GetViewMatrix();
	D3DXMATRIX mProj = *g_Camera.GetProjMatrix();

	XMFLOAT3 DircLightVector;
	DircLightVector.x = 0.0f;
	DircLightVector.y = 0.0f;
	DircLightVector.z = 0.0f;

	//////////////////////////////////// Object_Main_1_bunnytest //////////////////////////////////////////
	D3DXMATRIX scale; D3DXMatrixScaling(&scale, 3.5f, 3.0f, 3.0f);

	//revolution with Y axis
	D3DXMATRIX translation; //D3DXMatrixTranslation(&translation, 10.0f, 0.0f, 0.0f);

	static float moveSpeed1 = 15;
	static float moveSpeed2 = 10;
	static float moveSpeed3 = 17;

	if (fTime == 0) {
		if (moveSpeed1 <= 50.0f)
		{
			moveSpeed1 += 0.6f;
		}
		else
		{
			moveSpeed1 = 15.0f;
		}
	}

	D3DXMatrixTranslation(&translation, -moveSpeed1, 30.5f, -20.f);
	D3DXMATRIX mWorld = scale * translation;
	D3DXMATRIX mWorldView = mWorld * mView;
	D3DXMATRIX mWorldViewProjection = mWorld * mView * mProj;

	SetEffect(mWorld, mWorldView, mWorldViewProjection, g_pTextures_1, g_mPreProjectionMatrix1);
	RenderObject(g_pModel_1, pd3dImmediateContext, g_pLayout);
	g_mPreProjectionMatrix1 = mWorldViewProjection;

	////////////////////////////////////// Object_Main_2_test //////////////////////////////////////////

	D3DXMATRIX translation2;

	if (fTime == 0) {
		if (moveSpeed2 <= 50.0f)
		{
			moveSpeed2 += 0.6f;
		}
		else
		{
			moveSpeed2 = 10.0f;
		}
	}

	D3DXMatrixTranslation(&translation2, -moveSpeed2, 30.5f, -17.0f);

	D3DXMATRIX mWorld2 = scale * translation2;//
	D3DXMATRIX mWorldView2 = mWorld2 * mView;
	D3DXMATRIX mWorldViewProjection2 = mWorld2 * mView * mProj;

	SetEffect(mWorld2, mWorldView2, mWorldViewProjection2, g_pTextures_2, g_mPreProjectionMatrix2);
	RenderObject(g_pModel_2, pd3dImmediateContext, g_pLayout);
	g_mPreProjectionMatrix2 = mWorldViewProjection2;

	//////////////////////////////// Object_Main_3_test //////////////////////////////////////////
	// Configurate Lighting data

	float rotate = D3DXToRadian(90.0f);// here is change the angle

	D3DXMATRIX rotation3; D3DXMatrixRotationY(&rotation3, rotate);

	D3DXMATRIX translation3;
	if (fTime == 0) {
		if (moveSpeed3 <= 50.0f)
		{
			moveSpeed3 += 0.6f;
		}
		else
		{
			moveSpeed3 = 17.0f;
		}
	}

	D3DXMatrixTranslation(&translation3, -moveSpeed3, 30.5f, -23.0f);//view 2
	D3DXMATRIX mWorld3 = scale * translation3;//
	D3DXMATRIX mWorldView3 = mWorld3 * mView;
	D3DXMATRIX mWorldViewProjection3 = mWorld3 * mView * mProj;

	SetEffect(mWorld3, mWorldView3, mWorldViewProjection3, g_pTextures_3, g_mPreProjectionMatrix3);
	RenderObject(g_pModel_3, pd3dImmediateContext, g_pLayout);
	g_mPreProjectionMatrix3 = mWorldViewProjection3;

	/////////////////////////////////// Object_Plane_1_fruit ninja //////////////////////////////////////////
	// 
	// Prepare for Motion of object
	D3DXMATRIX translationPlane; D3DXMatrixTranslation(&translationPlane, 0.0f, 10.0f, -25.0f);//view 1

	D3DXMATRIX mWorldPlane = translationPlane;
	D3DXMATRIX mWorldViewProjectionPlane = mWorldPlane * mView * mProj;
	D3DXMATRIX mWorldViewPlane = mWorldPlane * mView;

	SetEffect(mWorldPlane, mWorldViewPlane, mWorldViewProjectionPlane, g_pTexturesPlane1, g_mPreProjectionMatrix7);
	RenderObject(g_pModelPlane1, pd3dImmediateContext, g_pLayout);
	g_mPreProjectionMatrix7 = mWorldViewProjectionPlane;

	static int globalTime = 0;
	globalTime += 1;

	if (globalTime >= 5 && globalTime <= 100)
	{
		CString fileName = "SHOT";
		CString x;
		x.Format(_T("%d"), globalTime);
		fileName += x;
		LPCTSTR name = L"./Screenshots/" + fileName + L".jpg";
		DXUTSnapD3D11Screenshot(name, D3DX11_IFF_JPG);
	}
}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext)
{
	g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice(void* pUserContext)
{
	g_DialogResourceManager.OnD3D11DestroyDevice();
	g_SettingsDlg.OnD3D11DestroyDevice();
	DXUTGetGlobalResourceCache().OnDestroyDevice();

	SAFE_DELETE(g_pTxtHelper);
	SAFE_RELEASE(g_pLayout);
	SAFE_RELEASE(g_pSamLinear);
	SAFE_RELEASE(g_pEffect);

	SAFE_RELEASE(g_pTextures_1);
	SAFE_RELEASE(g_pTextures_2);
	SAFE_RELEASE(g_pTextures_3);
	SAFE_RELEASE(g_pTexturesPlane1);

	SAFE_RELEASE(g_pcbVSPerObject_1);
	SAFE_RELEASE(g_pcbVSPerObject_2);
	SAFE_RELEASE(g_pcbVSPerObject_3);
	SAFE_RELEASE(g_pcbVSPerObjectPlane1);

	SAFE_RELEASE(g_pcbVSPerFrame_1);
	SAFE_RELEASE(g_pcbVSPerFrame_2);
	SAFE_RELEASE(g_pcbVSPerFrame_3);
	SAFE_RELEASE(g_pcbVSPerFramePlane1);

	delete g_pModel_1;
	delete g_pModel_2;
	delete g_pModel_3;
	delete g_pModelPlane1;

	for (int i = 0; i < 7; i++) {

		g_color_Map_RenderClass[i]->Shutdown();
		delete g_color_Map_RenderClass[i];
		g_color_Map_RenderClass[i] = 0;

		g_motionVector3D_Map_RenderClass[i]->Shutdown();
		delete g_motionVector3D_Map_RenderClass[i];
		g_motionVector3D_Map_RenderClass[i] = 0;

		g_motionVector3DSquare_Map_RenderClass[i]->Shutdown();
		delete g_motionVector3DSquare_Map_RenderClass[i];
		g_motionVector3DSquare_Map_RenderClass[i] = 0;

		g_previousDepth_Map_RenderClass[i]->Shutdown();
		delete g_previousDepth_Map_RenderClass[i];
		g_previousDepth_Map_RenderClass[i] = 0;

	}

	/*if (g_color_MapCur_RenderClass)
	{
		g_color_MapCur_RenderClass->Shutdown();
		delete g_color_MapCur_RenderClass;
		g_color_MapCur_RenderClass = 0;
	}
	if (g_motionVector3D_MapCur_RenderClass)
	{
		g_motionVector3D_MapCur_RenderClass->Shutdown();
		delete g_motionVector3D_MapCur_RenderClass;
		g_motionVector3D_MapCur_RenderClass = 0;
	}
	if (g_motionVector3DSquare_MapCur_RenderClass)
	{
		g_motionVector3DSquare_MapCur_RenderClass->Shutdown();
		delete g_motionVector3DSquare_MapCur_RenderClass;
		g_motionVector3DSquare_MapCur_RenderClass = 0;
	}
	if (g_previousDepth_MapCur_RenderClass)
	{
		g_previousDepth_MapCur_RenderClass->Shutdown();
		delete g_previousDepth_MapCur_RenderClass;
		g_previousDepth_MapCur_RenderClass = 0;
	}*/

	if (g_fullScreenQuad_Map_RenderClass)
	{
		g_fullScreenQuad_Map_RenderClass->Shutdown();
		delete g_fullScreenQuad_Map_RenderClass;
		g_fullScreenQuad_Map_RenderClass = 0;
	}

	if (g_fullScreenQuadDenoising_Map_RenderClass)
	{
		g_fullScreenQuadDenoising_Map_RenderClass->Shutdown();
		delete g_fullScreenQuadDenoising_Map_RenderClass;
		g_fullScreenQuadDenoising_Map_RenderClass = 0;
	}

	//////////////////// from Steve /////////////////////
	ID3D11Device* pd3dDevice = DXUTGetD3D11Device();
	ID3D11Debug* pDebug = NULL;
	HRESULT hr = pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pDebug));
	hr = pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	SAFE_RELEASE(pDebug);
}

//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D11 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	if (pDeviceSettings->ver == DXUT_D3D9_DEVICE)
	{
		IDirect3D9* pD3D = DXUTGetD3D9Object();
		D3DCAPS9 Caps;
		pD3D->GetDeviceCaps(pDeviceSettings->d3d9.AdapterOrdinal, pDeviceSettings->d3d9.DeviceType, &Caps);

		// If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
		// then switch to SWVP.
		if ((Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
			Caps.VertexShaderVersion < D3DVS_VERSION(1, 1))
		{
			pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}

		// Debugging vertex shaders requires either REF or software vertex processing 
		// and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
		if (pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF)
		{
			pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
			pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
			pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}
#endif
#ifdef DEBUG_PS
		pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
	}

	// For the first device created if its a REF device, optionally display a warning dialog box
	static bool s_bFirstTime = true;
	if (s_bFirstTime)
	{
		s_bFirstTime = false;
		if ((DXUT_D3D9_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF) ||
			(DXUT_D3D11_DEVICE == pDeviceSettings->ver &&
				pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE))
		{
			DXUTDisplaySwitchingToREFWarning(pDeviceSettings->ver);
		}

	}

	return true;
}

//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	// Update the camera's position based on user input 
	g_Camera.FrameMove(fElapsedTime);
}



//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext)
{
	// Pass messages to dialog resource manager calls so GUI state is updated correctly
	*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;
	// Pass messages to settings dialog if its active
	if (g_SettingsDlg.IsActive())
	{
		g_SettingsDlg.MsgProc(hWnd, uMsg, wParam, lParam);
		return 0;
	}
	// Pass all remaining windows messages to camera so it can respond to user input
	g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);
	return 0;
}

//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
	//char filePath[DEMO_MAX_FILEPATH];

	switch (nChar)
	{
	case 'C':
	{
		/*for (unsigned int i = 0; i < 1000; i++)
		{
			sprintf_s(filePath, "/Screenshots/screenshot%d.bmp", i + 1);
			if (!FilePathExists(filePath)) break;
			if (i == 999) return;
		}*/

		//LPCTSTR pszCharacterString = CA2W(filePath);

		DXUTSnapD3D11Screenshot(L"./Screenshots/TIME.bmp", D3DX11_IFF_BMP);
		break;
	}
	case 'V':
	{
		/*for (unsigned int i = 0; i < 1000; i++)
		{
			sprintf_s(filePath, "/Screenshots/screenshot%d.bmp", i + 1);
			if (!FilePathExists(filePath)) break;
			if (i == 999) return;
		}*/

		//LPCTSTR pszCharacterString = CA2W(filePath);

		DXUTSnapD3D11Screenshot(L"./Screenshots/screenshot2.bmp", D3DX11_IFF_BMP);
		break;
	}

	}


}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
	switch (nControlID)
	{
	case IDC_TOGGLEFULLSCREEN:
		DXUTToggleFullScreen();
		break;
	case IDC_TOGGLEREF:
		DXUTToggleREF();
		break;
	case IDC_CHANGEDEVICE:
		g_SettingsDlg.SetActive(!g_SettingsDlg.IsActive());
		break;
	}
}
