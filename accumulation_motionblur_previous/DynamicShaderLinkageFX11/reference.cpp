//--------------------------------------------------------------------------------------
// File: DyanmicShaderLinkageFX11.cpp
//
// This sample shows a simple example of the Microsoft Direct3D's High-Level 
// Shader Language (HLSL) using Dynamic Shader Linkage in conjunction
// with Effects 11.
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

#include <d3dcompiler.h>
#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"
#include "SDKMesh.h"
#include "resource.h"
#include "renderedObject.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <D3DX11tex.h>

#include "LightHelper.h"
#include "rendertextureclass.h"
#include "RenderHelper.h"
#include <atlstr.h> 
#include <string>

using namespace DirectX;
using namespace PackedVector;


#pragma warning( disable : 4100 )

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
CDXUTDialogResourceManager  g_DialogResourceManager;

CFirstPersonCamera                      g_Camera;

CD3DSettingsDlg             g_D3DSettingsDlg;       
CDXUTDialog                 g_HUD;                  
CDXUTDialog                 g_SampleUI;             

// Resources
CDXUTTextHelper* g_pTxtHelper = nullptr;

ID3D10Blob* effectBlob = NULL;
ID3D10Blob* errorsBlob = NULL;

CD3DSettingsDlg g_SettingsDlg;
ID3D11InputLayout* g_pLayout = NULL;

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
Animation* g_pFSQ = NULL;

XMMATRIX g_mPreProjectionMatrix1;
XMMATRIX g_mPreProjectionMatrix2;
XMMATRIX g_mPreProjectionMatrix3;
XMMATRIX g_mPreProjectionMatrix7;

RenderTextureClass* g_1at8Frame_Class;
RenderTextureClass* g_9at16Frame_Class;
RenderTextureClass* g_17at24Frame_Class;
RenderTextureClass* g_25at32Frame_Class;
bool isMove = true;

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext);
void CALLBACK OnKeyboard (UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo* AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo* DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext);
void CALLBACK OnD3D11DestroyDevice(void* pUserContext);
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext);

void Rendering(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech);
void InitApp();
void LoadModels(ID3D11Device* pd3dDevice);
void RenderText();
void SetResouce();
void renderTagetToImageFile(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11RenderTargetView* renderTaget, int frmae, CString fileName);
//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    // Enable run-time memory check for debug builds.
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    DXUTSetCallbackMsgProc(MsgProc);
    DXUTSetCallbackKeyboard(OnKeyboard);
    DXUTSetCallbackFrameMove(OnFrameMove);
    DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
    DXUTSetCallbackD3D11DeviceAcceptable(IsD3D11DeviceAcceptable);
    DXUTSetCallbackD3D11DeviceCreated(OnD3D11CreateDevice);
    DXUTSetCallbackD3D11SwapChainResized(OnD3D11ResizedSwapChain);
    DXUTSetCallbackD3D11SwapChainReleasing(OnD3D11ReleasingSwapChain);
    DXUTSetCallbackD3D11DeviceDestroyed(OnD3D11DestroyDevice);
    DXUTSetCallbackD3D11FrameRender(OnD3D11FrameRender);

    InitApp();
    DXUTInit(true, true, nullptr); 
    DXUTSetCursorSettings(true, true); 
    DXUTCreateWindow(L"reference");
    DXUTCreateDevice(D3D_FEATURE_LEVEL_11_0, true, SCREENWIDTH, SCREENHEIGHT);
    DXUTMainLoop(); 

    return DXUTGetExitCode();
}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    // Initialize dialogs
    g_D3DSettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );
}

//--------------------------------------------------------------------------------------
// Called right before creating a D3D device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
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
// Render the help and statistics text
//--------------------------------------------------------------------------------------
void RenderText()
{
    g_pTxtHelper->Begin();
    g_pTxtHelper->SetInsertionPos(2, 0);
    g_pTxtHelper->SetForegroundColor(Colors::Yellow);
    g_pTxtHelper->DrawTextLine(DXUTGetFrameStats(DXUTIsVsyncEnabled()));
    g_pTxtHelper->DrawTextLine(DXUTGetDeviceStats());

    g_pTxtHelper->End();
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
    if (g_D3DSettingsDlg.IsActive())
    {
        g_D3DSettingsDlg.MsgProc(hWnd, uMsg, wParam, lParam);
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc(hWnd, uMsg, wParam, lParam);
    if (*pbNoFurtherProcessing)
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc(hWnd, uMsg, wParam, lParam);
    if (*pbNoFurtherProcessing)
        return 0;

    //g_LightControl.HandleMessages( hWnd, uMsg, wParam, lParam );

    // Pass all remaining windows messages to camera so it can respond to user input
    g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);

    return 0;

}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
    if (bKeyDown)
    {
        switch (nChar)
        {
        case 'P':

            isMove = true;

            break;

        case 'O':

            isMove = false;

            break;
        }
    }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
}

//--------------------------------------------------------------------------------------
// Reject any D3D devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo* AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo* DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
    return true;
}

# pragma region Load Models and texture
void LoadModels(ID3D11Device* pd3dDevice)
{
    g_pModel_1 = new Animation();
    g_pModel_2 = new Animation();
    g_pModel_3 = new Animation();
    g_pModelPlane1 = new Animation();
    g_pFSQ = new Animation();

    g_pModel_1->LoadMesh("./models/Sphere/sphere.x", pd3dDevice);
    g_pModel_2->LoadMesh("./models/sphere/sphere.x", pd3dDevice);
    g_pModel_3->LoadMesh("./models/sphere/sphere.x", pd3dDevice);
    g_pModelPlane1->LoadMesh("./Models/table.obj", pd3dDevice);

    g_pFSQ->LoadMesh("./models/sphere/test2.obj", pd3dDevice);
}

HRESULT LoadTextures(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
    HRESULT hr;

    V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"./models/Sphere/billiard13.jpg", &g_pTextures_1));
    V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"./models/Sphere/red.jpg", &g_pTextures_2));
    V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"./models/Sphere/billiard14.jpg", &g_pTextures_3));

    V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"./Models/bilispoyta2.png", &g_pTexturesPlane1));
}
# pragma endregion

//--------------------------------------------------------------------------------------
// Create any D3D resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,void* pUserContext)
{
    HRESULT hr = S_OK;

    auto pd3dImmediateContext = DXUTGetD3D11DeviceContext();
    V_RETURN(g_DialogResourceManager.OnD3D11CreateDevice(pd3dDevice, pd3dImmediateContext));
    V_RETURN(g_D3DSettingsDlg.OnD3D11CreateDevice(pd3dDevice));
    g_pTxtHelper = new CDXUTTextHelper(pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15);

    // Compile and create the effect.
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
    dwShaderFlags |= D3DCOMPILE_DEBUG;
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

    hr = D3DX11CompileFromFile(L"reference.fx", 0, 0, 0, "fx_5_0", dwShaderFlags, 0, 0, &effectBlob, &errorsBlob, 0);

    if (errorsBlob)
    {
        MessageBoxA(0, (char*)errorsBlob->GetBufferPointer(), 0, 0);
        errorsBlob->Release();
    }

    hr = D3DX11CreateEffectFromMemory(effectBlob->GetBufferPointer(), effectBlob->GetBufferSize(), 0, pd3dDevice, &g_pEffect);
    assert(SUCCEEDED(hr));
    effectBlob->Release();

    g_pRendering_Tech = g_pEffect->GetTechniqueByName("Rendering");
    g_pMotionBlur_Tech = g_pEffect->GetTechniqueByName("motionBlur");

    D3DX11_PASS_DESC passDesc;
    g_pRendering_Tech->GetPassByIndex(0)->GetDesc(&passDesc);

    const D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    V_RETURN(pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &g_pLayout));

    // Create constant buffers of Main Object 1
    D3D11_BUFFER_DESC cbDesc_1;
    ZeroMemory(&cbDesc_1, sizeof(cbDesc_1));
    cbDesc_1.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc_1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc_1.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc_1.ByteWidth = sizeof(CB_VS_PER_OBJECT);
    V_RETURN(pd3dDevice->CreateBuffer(&cbDesc_1, NULL, &g_pcbVSPerObject_1));
    DXUT_SetDebugName(g_pcbVSPerObject_1, "CB_VS_PER_OBJECT");
    cbDesc_1.ByteWidth = sizeof(CB_VS_PER_Frame);
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
    cbDesc_2.ByteWidth = sizeof(CB_VS_PER_Frame);
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
    cbDesc_3.ByteWidth = sizeof(CB_VS_PER_Frame);
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
    cbDescPlane1.ByteWidth = sizeof(CB_VS_PER_Frame);
    V_RETURN(pd3dDevice->CreateBuffer(&cbDescPlane1, NULL, &g_pcbVSPerFramePlane1));
    DXUT_SetDebugName(g_pcbVSPerFramePlane1, "CB_VS_PER_FRAME_PLANE_1");

    LoadModels(pd3dDevice);
    LoadTextures(pd3dDevice, pd3dImmediateContext);

    g_1at8Frame_Class = new RenderTextureClass;
    g_1at8Frame_Class ->Initialize(pd3dDevice, SCREENWIDTH, SCREENHEIGHT, RENDER_TARGET_ARRAY);

    g_9at16Frame_Class = new RenderTextureClass;
    g_9at16Frame_Class ->Initialize(pd3dDevice, SCREENWIDTH, SCREENHEIGHT, RENDER_TARGET_ARRAY);

    g_17at24Frame_Class = new RenderTextureClass;
    g_17at24Frame_Class ->Initialize(pd3dDevice, SCREENWIDTH, SCREENHEIGHT, RENDER_TARGET_ARRAY);

    g_25at32Frame_Class = new RenderTextureClass;
    g_25at32Frame_Class ->Initialize(pd3dDevice, SCREENWIDTH, SCREENHEIGHT, RENDER_TARGET_ARRAY);


    const XMVECTORF32 vecEye = { -31.0f, 36.0f, -0.43f, 0.f };
    const XMVECTORF32 vecAt = { -31.1f, 35.6f, -1.3f, 0.f };

    g_Camera.SetViewParams(vecEye, vecAt);

    GetEffect();
    SetResouce();

    return hr;
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
    const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
    HRESULT hr = S_OK;

    V_RETURN(g_DialogResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));
    V_RETURN(g_D3DSettingsDlg.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));
        
    constexpr float fAspectRatio = SCREENWIDTH / (FLOAT)SCREENHEIGHT;
    g_Camera.SetProjParams(XM_PI / 4, fAspectRatio, ZNEAR, ZFAR);
    
    g_HUD.SetLocation(SCREENWIDTH - 170, 0);
    g_HUD.SetSize(170, 170);
    g_SampleUI.SetLocation(SCREENWIDTH - 170, SCREENHEIGHT - 300);
    g_SampleUI.SetSize(170, 300);

    return hr;
}

void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext)
{
    if (g_D3DSettingsDlg.IsActive())
    {
        g_D3DSettingsDlg.OnRender(fElapsedTime);
        return;
    }

    const FLOAT ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    D3D11_VIEWPORT mViewport;
    ZeroMemory(&mViewport, sizeof(mViewport));
    mViewport.TopLeftX = 0.0f;
    mViewport.TopLeftY = 0.0f;
    mViewport.Width = SCREENWIDTH;
    mViewport.Height = SCREENHEIGHT;
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;
    pd3dImmediateContext->RSSetViewports(1, &mViewport);
    pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ID3D11DepthStencilState* m_depthStencilState = NULL;

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.StencilEnable = false;
    pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);

    pd3dImmediateContext->OMSetDepthStencilState(m_depthStencilState, NULL);
    
    //------------------------------------------------------------- rendering

    ID3D11DepthStencilView* FrameDSV = g_1at8Frame_Class->GetDepthStencilView();
    ID3D11RenderTargetView* FrameRTV1At8[TEXTUREARRSIZE] = {};
     
    for (int i = 0; i < TEXTUREARRSIZE; i++) {
        FrameRTV1At8[i] = g_1at8Frame_Class->GetRenderTargetViewArray(i);

        pd3dImmediateContext->ClearRenderTargetView(FrameRTV1At8[i], ClearColor);
        pd3dImmediateContext->OMSetRenderTargets(1, &FrameRTV1At8[i], FrameDSV);
        pd3dImmediateContext->ClearDepthStencilView(FrameDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        Rendering(pd3dDevice, pd3dImmediateContext, g_pRendering_Tech);
    }
    
    if (MAXSUBFRAME > 8) {
        ID3D11RenderTargetView* FrameRTV9at16[TEXTUREARRSIZE];
        for (int i = 0; i < TEXTUREARRSIZE; i++) {
            FrameRTV9at16[i] = g_9at16Frame_Class->GetRenderTargetViewArray(i);

            pd3dImmediateContext->ClearRenderTargetView(FrameRTV9at16[i], ClearColor);
            pd3dImmediateContext->OMSetRenderTargets(1, &FrameRTV9at16[i], FrameDSV);
            pd3dImmediateContext->ClearDepthStencilView(FrameDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

            Rendering(pd3dDevice, pd3dImmediateContext, g_pRendering_Tech);
        }
    }
    if (MAXSUBFRAME > 16) {
        ID3D11RenderTargetView* FrameRTV17at24[TEXTUREARRSIZE];
        for (int i = 0; i < TEXTUREARRSIZE; i++) {
            FrameRTV17at24[i] = g_17at24Frame_Class->GetRenderTargetViewArray(i);

            pd3dImmediateContext->ClearRenderTargetView(FrameRTV17at24[i], ClearColor);
            pd3dImmediateContext->OMSetRenderTargets(1, &FrameRTV17at24[i], FrameDSV);
            pd3dImmediateContext->ClearDepthStencilView(FrameDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

            Rendering(pd3dDevice, pd3dImmediateContext, g_pRendering_Tech);

        }
    }
    if (MAXSUBFRAME > 24) {
        ID3D11RenderTargetView* FrameRTV25at32[TEXTUREARRSIZE];
        for (int i = 0; i < TEXTUREARRSIZE; i++) {
            FrameRTV25at32[i] = g_25at32Frame_Class->GetRenderTargetViewArray(i);

            pd3dImmediateContext->ClearRenderTargetView(FrameRTV25at32[i], ClearColor);
            pd3dImmediateContext->OMSetRenderTargets(1, &FrameRTV25at32[i], FrameDSV);
            pd3dImmediateContext->ClearDepthStencilView(FrameDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

            Rendering(pd3dDevice, pd3dImmediateContext, g_pRendering_Tech);

        }
    }
    ID3D11RenderTargetView* pDXUTRTV = DXUTGetD3D11RenderTargetView();
    pd3dImmediateContext->ClearRenderTargetView(pDXUTRTV, ClearColor);
    pd3dImmediateContext->OMSetRenderTargets(1, &pDXUTRTV, NULL);

    RenderObject(g_pFSQ, pd3dImmediateContext, g_pLayout, g_pMotionBlur_Tech);
    //g_pDenoising_Tech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
    //pd3dImmediateContext->Draw(3, 0);

    //static int frame = 0;
    //CString fileName = "reference";
    //if (frame <= 200) renderTagetToImageFile(pd3dDevice, pd3dImmediateContext, pDXUTRTV, frame, fileName);
    //frame++;

    RenderText();
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void Rendering(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech)
{
    // Prepare for Motion of object
    const XMMATRIX mView = g_Camera.GetViewMatrix();
    const XMMATRIX mProj = g_Camera.GetProjMatrix();

    static float movePos_1 = 15.f + MOVESPEED / 2;
    static float movePos_2 = 50.f - MOVESPEED / 2;
    static float movePos_3 = 17.f + MOVESPEED / 2;

    //////////////////////////////////// Object_Main_1_bunnytest //////////////////////////////////////////

    const XMMATRIX scale = XMMatrixScaling(3.5f, 3.0f, 3.0f);

    if (isMove)
    {
        if (movePos_2 <= 50.0f)
        {
            movePos_2 += MOVESPEED;
        }
        else
        {
            movePos_2 = 15.0f;
        }
    }

    const XMMATRIX translation = XMMatrixTranslation(-movePos_2, 30.5f, -20.0f);
    const XMMATRIX mWorld = scale * translation;
    const XMMATRIX mWorldViewProjection = mWorld * mView * mProj;

    SetEffect(mWorldViewProjection, g_pTextures_1, g_mPreProjectionMatrix1);
    RenderObject(g_pModel_1, pd3dImmediateContext, g_pLayout, pTech);

    //////////////////////////////////// Object_Main_2_test //////////////////////////////////////////

    if (isMove)
    {
        if (movePos_1 > 10.0f)
        {
            movePos_1 -= MOVESPEED;
        }
        else
        {
            movePos_1 = 50.0f;
        }
    }
    const XMMATRIX translation2 = XMMatrixTranslation(-movePos_1, 30.5f, -17.0f);
    const XMMATRIX mWorld2 = scale * translation2;
    const XMMATRIX mWorldViewProjection2 = mWorld2 * mView * mProj;

    SetEffect(mWorldViewProjection2, g_pTextures_2, g_mPreProjectionMatrix2);
    RenderObject(g_pModel_2, pd3dImmediateContext, g_pLayout, pTech);

    ////////////////////////////////// Object_Main_3_test //////////////////////////////////////////

    if (isMove)
    {
        if (movePos_3 <= 50.0f)
        {
            movePos_3 += MOVESPEED;
        }
        else
        {
            movePos_3 = 17.0f;
        }
    }

    const XMMATRIX translation3 = XMMatrixTranslation(-movePos_3, 30.5f, -23.0f);
    const XMMATRIX mWorld3 = scale * translation3;
    const XMMATRIX mWorldViewProjection3 = mWorld3 * mView * mProj;

    SetEffect(mWorldViewProjection3, g_pTextures_3, g_mPreProjectionMatrix3);
    RenderObject(g_pModel_3, pd3dImmediateContext, g_pLayout, pTech);


    /////////////////////////////////// Object_Plane //////////////////////////////////////////

    const XMMATRIX mWorldPlane = XMMatrixTranslation(0.0f, 10.0f, -25.0f);//view 1
    const XMMATRIX mWorldViewProjectionPlane = mWorldPlane * mView * mProj;

    SetEffect(mWorldViewProjectionPlane, g_pTexturesPlane1, g_mPreProjectionMatrix7);
    RenderObject(g_pModelPlane1, pd3dImmediateContext, g_pLayout, pTech);
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
    g_D3DSettingsDlg.OnD3D11DestroyDevice();
    CDXUTDirectionWidget::StaticOnD3D11DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();
    SAFE_DELETE( g_pTxtHelper );

    g_SettingsDlg.OnD3D11DestroyDevice();

    SAFE_DELETE(g_pTxtHelper);
    SAFE_RELEASE(g_pLayout);
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
    delete g_pModelPlane1;/*

    g_1at8Frame_Class->Shutdown(RENDER_TARGET_ARRAY);
    delete[] g_1at8Frame_Class;
    g_1at8Frame_Class = 0;

    g_9at16Frame_Class->Shutdown(RENDER_TARGET_ARRAY);  
    delete[] g_9at16Frame_Class;
    g_9at16Frame_Class = 0;

    g_17at24Frame_Class->Shutdown(RENDER_TARGET_ARRAY);
    delete[] g_17at24Frame_Class;
    g_17at24Frame_Class = 0;

    g_25at32Frame_Class->Shutdown(RENDER_TARGET_ARRAY);    
    delete[] g_25at32Frame_Class;
    g_25at32Frame_Class = 0;*/
}

void SetResouce() {
    for (int i = 0; i < TEXTUREARRSIZE; i++)	g_1at8Frame->SetResource(g_1at8Frame_Class->GetShaderResourceViewArray(i));
    for (int i = 0; i < TEXTUREARRSIZE; i++)	g_9at16Frame->SetResource(g_9at16Frame_Class->GetShaderResourceViewArray(i));
    for (int i = 0; i < TEXTUREARRSIZE; i++)	g_17at24Frame->SetResource(g_17at24Frame_Class->GetShaderResourceViewArray(i));
    for (int i = 0; i < TEXTUREARRSIZE; i++)	g_25at32Frame->SetResource(g_25at32Frame_Class->GetShaderResourceViewArray(i));
}

void renderTagetToImageFile(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11RenderTargetView* renderTaget, int frmae, CString fileName) {
    ID3D11Resource* backbuffer;
    renderTaget->GetResource(&backbuffer);

    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.ArraySize = 1;
    texDesc.BindFlags = 0;
    texDesc.CPUAccessFlags = 0;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.Width = SCREENWIDTH;
    texDesc.Height = SCREENHEIGHT;
    texDesc.MipLevels = 1;
    texDesc.MiscFlags = 0;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;

    ID3D11Texture2D* texture;
    pd3dDevice->CreateTexture2D(&texDesc, 0, &texture);
    pd3dImmediateContext->CopyResource(texture, backbuffer);

    CString sFrame;
    sFrame.Format(_T("%d"), frmae);
    fileName += sFrame;
    LPCTSTR name = L"./Screenshots/" + fileName + L".bmp";

    D3DX11SaveTextureToFile(pd3dImmediateContext, texture, D3DX11_IFF_BMP, name);
    texture->Release();
    backbuffer->Release();
}