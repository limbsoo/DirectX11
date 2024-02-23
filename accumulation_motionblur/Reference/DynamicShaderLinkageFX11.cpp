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


#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"
#include "SDKMesh.h"
#include "resource.h"
#include <d3dx11effect.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <D3DX11tex.h>

#include <RenderObject.h>
#include "RenderTexture.h"
#include "define.h"
//#include "RenderHelper.h"

#include <atlstr.h> 
#include <string>

#include"ScreenGrab.h"
#include <d3dcompiler.h>

using namespace DirectX;
using namespace PackedVector;

#pragma warning( disable : 4100 )

// We show two ways of handling dynamic linkage binding.
// This #define selects between a single technique where
// bindings are done via effect variables and multiple
// techniques where the bindings are done with BindInterfaces
// in the technqiues.
#define USE_BIND_INTERFACES 0

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs

//CModelViewerCamera        g_Camera;               // A model viewing camera
CFirstPersonCamera          g_Camera;
CD3DSettingsDlg             g_D3DSettingsDlg;       // Device settings dialog
CDXUTDialog                 g_HUD;                  // manages the 3D   
CDXUTDialog                 g_SampleUI;             // dialog for sample specific controls

// Resources
CDXUTTextHelper* g_pTxtHelper = nullptr;

ID3D10Blob* g_pEffectBlob = NULL;
ID3D10Blob* g_pErrorsBlob = NULL;

ID3D11InputLayout* g_pLayout = NULL;

RenderTexture* g_Ref_Primary_pColorMap_RenderingClass;
RenderTexture* g_Ref_Secondary_pColorMap_RenderingClass;
RenderTexture* g_Ref_Tertiary_pColorMap_RenderingClass;
RenderTexture* g_Ref_Quaternary_pColorMap_RenderingClass;
RenderTexture* g_Ref_pDepthBuffer_RenderingClass;


RenderTexture* g_Ref_5_RenderingClass;
RenderTexture* g_Ref_6_RenderingClass;
RenderTexture* g_Ref_7_RenderingClass;
RenderTexture* g_Ref_8_RenderingClass;


//ModelClass 
RenderObject* g_pModel_1 = new RenderObject;
RenderObject* g_pModel_2 = new RenderObject;
RenderObject* g_pModel_3 = new RenderObject;
RenderObject* g_pModel_4 = new RenderObject;
RenderObject* g_pBackgroundModel = new RenderObject;
RenderObject* g_pFSQ = new RenderObject;


bool g_bShowHelp = false;    // If true, it renders the UI control text
bool g_isMove = true;
int g_globalTime = 0;


ID3DX11Effect* g_pEffect = NULL;
ID3DX11EffectTechnique* g_pTech = NULL;
D3DX11_TECHNIQUE_DESC techDesc;

//--------------------------------------
//      Reference
//--------------------------------------

ID3DX11EffectShaderResourceVariable* g_Ref_Primary_effectSRV_colorMap;
ID3DX11EffectShaderResourceVariable* g_Ref_Secondary_effectSRV_colorMap;
ID3DX11EffectShaderResourceVariable* g_Ref_Tertiary_effectSRV_colorMap;
ID3DX11EffectShaderResourceVariable* g_Ref_Quaternary_effectSRV_colorMap;


ID3DX11EffectShaderResourceVariable* g_Ref_5_effectSRV_colorMap;
ID3DX11EffectShaderResourceVariable* g_Ref_6_effectSRV_colorMap;
ID3DX11EffectShaderResourceVariable* g_Ref_7_effectSRV_colorMap;
ID3DX11EffectShaderResourceVariable* g_Ref_8_effectSRV_colorMap;
                                     

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
# pragma region Forward declarations
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext);
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo* AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo* DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext);
void CALLBACK OnD3D11DestroyDevice(void* pUserContext);
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext);
void InitApp();


void renderTagetToImageFile(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11RenderTargetView* renderTaget, int frmae, CString fileName);

void LoadModels(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
void RenderText();
# pragma endregion

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

    DXUTInit(true, true, nullptr); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings(true, true); // Show the cursor and clip it when in full screen
    DXUTCreateWindow(L"DynamicShaderLinkageFX11");
    DXUTCreateDevice(D3D_FEATURE_LEVEL_11_0, true, SCREENWIDTH, SCREENHEIGHT);
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
    g_D3DSettingsDlg.Init(&g_DialogResourceManager);
    g_HUD.Init(&g_DialogResourceManager);
    g_HUD.SetCallback(OnGUIEvent);
    //g_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 170, 22);
    //g_HUD.AddButton(IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += iYo, 170, 22, VK_F3);
    //g_HUD.AddButton(IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += iYo, 170, 22, VK_F2);
    g_SampleUI.Init(&g_DialogResourceManager);
    g_SampleUI.SetCallback(OnGUIEvent); iY = 10;
    g_Camera.SetRotateButtons(true, false, false);

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
        case VK_F1:
            g_bShowHelp = !g_bShowHelp;
            break;

        case 'P':
            g_isMove = true;
            break;

        case 'O':
            g_isMove = false;
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
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo* AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo* DeviceInfo,
    DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
    return true;
}


# pragma region Load Models and texture
void LoadModels(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
    g_pFSQ->MakeLoadModel("./Models/city/FSQ.obj", pd3dDevice, pd3dImmediateContext);

//-----------------------------------------------------------------------------
// 	   City cars
//-----------------------------------------------------------------------------

    g_pModel_1->MakeLoadModel("./Objects/cars/car_1.obj", pd3dDevice, pd3dImmediateContext);
    g_pModel_2->MakeLoadModel("./Objects/cars/car_2.obj", pd3dDevice, pd3dImmediateContext);
    g_pModel_3->MakeLoadModel("./Objects/cars/car_3.obj", pd3dDevice, pd3dImmediateContext);
    g_pModel_4->MakeLoadModel("./Objects/cars/car_4.obj", pd3dDevice, pd3dImmediateContext);
    g_pBackgroundModel->MakeLoadModel("./Objects/city/123123444.obj", pd3dDevice, pd3dImmediateContext);
   
    //g_pBackgroundModel->MakeLoadModel("./Objects/city/poly2361457.obj", pd3dDevice, pd3dImmediateContext);
    //g_pBackgroundModel->MakeLoadModel("./Objects/city/poly1717250.obj", pd3dDevice, pd3dImmediateContext);

//-----------------------------------------------------------------------------
}



void renderPerObjects(ID3D11DeviceContext* pd3dImmediateContext)
{
    static float movePos_1 = 50.f;
    static float movePos_2 = 50.f;;
    static float movePos_3 = 50.f;;
    static float movePos_4 = -50.f;

    if (g_isMove)
    {
        movePos_1 = g_pModel_1->SetMovePosition(movePos_1, 0.5f / MAXSUBFRAME, 50.f, -50.f);
        movePos_2 = g_pModel_2->SetMovePosition(movePos_2, 0.5f / MAXSUBFRAME, 50.f, -50.f);
        movePos_3 = g_pModel_3->SetMovePosition(movePos_3, 0.5f / MAXSUBFRAME, 50.f, -50.f);
        movePos_4 = g_pModel_4->SetMovePosition(movePos_4, 0.5f / MAXSUBFRAME, -50.f, 50.f);
    }


    g_pModel_1->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 0.02f, -movePos_1, -0.0f, -0.f);
    g_pModel_2->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 0.02f, -movePos_2, -0.0f, -0.f);
    g_pModel_3->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 0.02f, -movePos_3, -0.0f, -0.f);
    g_pModel_4->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 0.02f, -movePos_4, -0.0f, -0.f);
    g_pBackgroundModel->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 0.02f, 0.0f, 0.0f, 0.f);

    g_pModel_1->Render(pd3dImmediateContext, g_pLayout, g_pTech, techDesc, g_pEffect);
    g_pModel_2->Render(pd3dImmediateContext, g_pLayout, g_pTech, techDesc, g_pEffect);
    g_pModel_3->Render(pd3dImmediateContext, g_pLayout, g_pTech, techDesc, g_pEffect);
    g_pModel_4->Render(pd3dImmediateContext, g_pLayout, g_pTech, techDesc, g_pEffect);
    g_pBackgroundModel->Render(pd3dImmediateContext, g_pLayout, g_pTech, techDesc, g_pEffect);

    g_pModel_1->AfterFrameRender();
    g_pModel_2->AfterFrameRender();
    g_pModel_3->AfterFrameRender();
    g_pModel_4->AfterFrameRender();
    g_pBackgroundModel->AfterFrameRender();
}


# pragma endregion

//--------------------------------------------------------------------------------------
// Create any D3D resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
    HRESULT hr = S_OK;

    ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
    V_RETURN(g_DialogResourceManager.OnD3D11CreateDevice(pd3dDevice, pd3dImmediateContext));
    V_RETURN(g_D3DSettingsDlg.OnD3D11CreateDevice(pd3dDevice));
    g_pTxtHelper = new CDXUTTextHelper(pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15);
    //DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
    dwShaderFlags |= D3DCOMPILE_DEBUG;
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

    hr = D3DX11CompileFromFile(L"DynamicShaderLinkageFX11.fx", 0, 0, 0, "fx_5_0", dwShaderFlags, 0, 0, &g_pEffectBlob, &g_pErrorsBlob, 0);
    assert(SUCCEEDED(hr) && g_pEffectBlob);

    if (g_pErrorsBlob)
    {
        MessageBoxA(0, (char*)g_pErrorsBlob->GetBufferPointer(), 0, 0);
        g_pErrorsBlob->Release();
    }

    hr = D3DX11CreateEffectFromMemory(g_pEffectBlob->GetBufferPointer(), g_pEffectBlob->GetBufferSize(), 0, pd3dDevice, &g_pEffect);
    assert(SUCCEEDED(hr));
    g_pEffectBlob->Release();

    D3DX11_PASS_DESC passDesc;
    g_pTech = g_pEffect->GetTechniqueByName("ReferenceRendering");
    g_pTech->GetPassByIndex(0)->GetDesc(&passDesc);

    const D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0  },    //12
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0  },
    };

    V_RETURN(pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &g_pLayout));


    LoadModels(pd3dDevice, pd3dImmediateContext);



    //--------------------------------------
    //      Reference
    //--------------------------------------


    //GetSRVEffect();
    g_Ref_Primary_effectSRV_colorMap = g_pEffect->GetVariableByName("g_Ref_Primary_colorMap")->AsShaderResource();
    g_Ref_Secondary_effectSRV_colorMap = g_pEffect->GetVariableByName("g_Ref_Secondary_colorMap")->AsShaderResource();
    g_Ref_Tertiary_effectSRV_colorMap = g_pEffect->GetVariableByName("g_Ref_Tertiary_colorMap")->AsShaderResource();
    g_Ref_Quaternary_effectSRV_colorMap = g_pEffect->GetVariableByName("g_Ref_Quaternary_colorMap")->AsShaderResource();


    //////////////////////////////
    g_Ref_5_effectSRV_colorMap = g_pEffect->GetVariableByName("g_Ref_5_colorMap")->AsShaderResource();
    g_Ref_6_effectSRV_colorMap = g_pEffect->GetVariableByName("g_Ref_6_colorMap")->AsShaderResource();
    g_Ref_7_effectSRV_colorMap = g_pEffect->GetVariableByName("g_Ref_7_colorMap")->AsShaderResource();
    g_Ref_8_effectSRV_colorMap = g_pEffect->GetVariableByName("g_Ref_8_colorMap")->AsShaderResource();

    //Initialize
    g_Ref_Primary_pColorMap_RenderingClass = new RenderTexture;
    g_Ref_Primary_pColorMap_RenderingClass->InitializeBackBufferWithMipMap_Array(pd3dDevice, SCREENWIDTH, SCREENHEIGHT);

    g_Ref_Secondary_pColorMap_RenderingClass = new RenderTexture;
    g_Ref_Secondary_pColorMap_RenderingClass->InitializeBackBufferWithMipMap_Array(pd3dDevice, SCREENWIDTH, SCREENHEIGHT);

    g_Ref_Tertiary_pColorMap_RenderingClass = new RenderTexture;
    g_Ref_Tertiary_pColorMap_RenderingClass->InitializeBackBufferWithMipMap_Array(pd3dDevice, SCREENWIDTH, SCREENHEIGHT);

    g_Ref_Quaternary_pColorMap_RenderingClass = new RenderTexture;
    g_Ref_Quaternary_pColorMap_RenderingClass->InitializeBackBufferWithMipMap_Array(pd3dDevice, SCREENWIDTH, SCREENHEIGHT);

    g_Ref_pDepthBuffer_RenderingClass = new RenderTexture;
    g_Ref_pDepthBuffer_RenderingClass->Initialize_DepthBuffer_DepthPeeling(pd3dDevice, SCREENWIDTH, SCREENHEIGHT);





    g_Ref_5_RenderingClass = new RenderTexture;
    g_Ref_5_RenderingClass->InitializeBackBufferWithMipMap_Array(pd3dDevice, SCREENWIDTH, SCREENHEIGHT);

    g_Ref_6_RenderingClass = new RenderTexture;
    g_Ref_6_RenderingClass->InitializeBackBufferWithMipMap_Array(pd3dDevice, SCREENWIDTH, SCREENHEIGHT);

    g_Ref_7_RenderingClass = new RenderTexture;
    g_Ref_7_RenderingClass->InitializeBackBufferWithMipMap_Array(pd3dDevice, SCREENWIDTH, SCREENHEIGHT);

    g_Ref_8_RenderingClass = new RenderTexture;
    g_Ref_8_RenderingClass->InitializeBackBufferWithMipMap_Array(pd3dDevice, SCREENWIDTH, SCREENHEIGHT);



//-----------------------------------------------------------------------------
// 	   City cars
//-----------------------------------------------------------------------------

    //XMVECTORF32 vecEye = { -3.0f, 2.f, 12.f, 0.f };
    //XMVECTORF32 vecAt = { -3.0f, 2.f, -1.f, 0.f };
    //g_Camera.SetViewParams(vecEye, vecAt);

    //XMVECTORF32 vecEye = { 25.0f, -26.f, 25.f, 0.f };
    //XMVECTORF32 vecAt = { 25.0f, -26.f, 0.f, 0.f };
    //g_Camera.SetViewParams(vecEye, vecAt);

    //XMVECTORF32 vecEye = { 31.0f, -26.f, 18.f, 0.f };
    //XMVECTORF32 vecAt = { 31.0f, -26.f, 0.f, 0.f };
    //g_Camera.SetViewParams(vecEye, vecAt);

    const XMVECTORF32 vecEye = { 5.0f, -26.f, 16.f, 0.f };
    const XMVECTORF32 vecAt = { 17.0f, -26.f, 0.f, 0.f };
    g_Camera.SetViewParams(vecEye, vecAt);

    return hr;

}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
    const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
    HRESULT hr;
    V_RETURN(g_DialogResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));
    V_RETURN(g_D3DSettingsDlg.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));
    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams(XM_PI / 4, fAspectRatio, 0.1f, 1000.0f);
    g_HUD.SetLocation(pBackBufferSurfaceDesc->Width - 170, 0);
    g_HUD.SetSize(170, 170);
    g_SampleUI.SetLocation(pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300);
    g_SampleUI.SetSize(170, 300);
    return S_OK;
}

void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext)
{
    HRESULT hr;


//-----------------------------------------------------------------------------
// 	   City cars
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

    // If the settings dialog is being shown, then render it instead of rendering the app's scene
    if (g_D3DSettingsDlg.IsActive())
    {
        g_D3DSettingsDlg.OnRender(fElapsedTime);
        return;
    }

    D3D11_VIEWPORT mViewport;
    ZeroMemory(&mViewport, sizeof(mViewport));
    mViewport.TopLeftX = 0.0f;
    mViewport.TopLeftY = 0.0f;
    mViewport.Width = static_cast<float>(1920);
    mViewport.Height = static_cast<float>(1080);
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;
    pd3dImmediateContext->RSSetViewports(1, &mViewport);

    float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

//-----------------------------------------------------------------------------
// 	  Reference: Render reference per layer
//-----------------------------------------------------------------------------


    g_pTech = g_pEffect->GetTechniqueByName("ReferenceRendering");
    g_pTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);

    for (int layer = 0; layer < N_LAYER; layer++)
    {
        pd3dImmediateContext->ClearRenderTargetView(g_Ref_Primary_pColorMap_RenderingClass->ppRTVs[layer], ClearColor);
        pd3dImmediateContext->ClearDepthStencilView(g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0, 0);
        pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_Primary_pColorMap_RenderingClass->ppRTVs[layer], NULL);
        pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_Primary_pColorMap_RenderingClass->ppRTVs[layer], g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView());
        renderPerObjects(pd3dImmediateContext);

    }

    g_Ref_Primary_effectSRV_colorMap->SetResource(g_Ref_Primary_pColorMap_RenderingClass->ppSRVs[0]);

    if (MAXSUBFRAME > 8)
    {
        for (int layer = 0; layer < N_LAYER; layer++)
        {
            pd3dImmediateContext->ClearRenderTargetView(g_Ref_Secondary_pColorMap_RenderingClass->ppRTVs[layer], ClearColor);
            pd3dImmediateContext->ClearDepthStencilView(g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0, 0);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_Secondary_pColorMap_RenderingClass->ppRTVs[layer], NULL);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_Secondary_pColorMap_RenderingClass->ppRTVs[layer], g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView());
            renderPerObjects(pd3dImmediateContext);

        }

        g_Ref_Secondary_effectSRV_colorMap->SetResource(g_Ref_Secondary_pColorMap_RenderingClass->ppSRVs[0]);
    }

    if (MAXSUBFRAME > 16)
    {
        for (int layer = 0; layer < N_LAYER; layer++)
        {
            pd3dImmediateContext->ClearRenderTargetView(g_Ref_Tertiary_pColorMap_RenderingClass->ppRTVs[layer], ClearColor);
            pd3dImmediateContext->ClearDepthStencilView(g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0, 0);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_Tertiary_pColorMap_RenderingClass->ppRTVs[layer], NULL);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_Tertiary_pColorMap_RenderingClass->ppRTVs[layer], g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView());
            renderPerObjects(pd3dImmediateContext);

        }

        g_Ref_Tertiary_effectSRV_colorMap->SetResource(g_Ref_Tertiary_pColorMap_RenderingClass->ppSRVs[0]);
    }

    if (MAXSUBFRAME > 24)
    {
        for (int layer = 0; layer < N_LAYER; layer++)
        {
            pd3dImmediateContext->ClearRenderTargetView(g_Ref_Quaternary_pColorMap_RenderingClass->ppRTVs[layer], ClearColor);
            pd3dImmediateContext->ClearDepthStencilView(g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0, 0);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_Quaternary_pColorMap_RenderingClass->ppRTVs[layer], NULL);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_Quaternary_pColorMap_RenderingClass->ppRTVs[layer], g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView());
            renderPerObjects(pd3dImmediateContext);

        }

        g_Ref_Quaternary_effectSRV_colorMap->SetResource(g_Ref_Quaternary_pColorMap_RenderingClass->ppSRVs[0]);
    }






    if (MAXSUBFRAME > 32)
    {
        for (int layer = 0; layer < N_LAYER; layer++)
        {
            pd3dImmediateContext->ClearRenderTargetView(g_Ref_5_RenderingClass->ppRTVs[layer], ClearColor);
            pd3dImmediateContext->ClearDepthStencilView(g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0, 0);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_5_RenderingClass->ppRTVs[layer], NULL);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_5_RenderingClass->ppRTVs[layer], g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView());
            renderPerObjects(pd3dImmediateContext);

        }

        g_Ref_5_effectSRV_colorMap->SetResource(g_Ref_5_RenderingClass->ppSRVs[0]);
    }

    if (MAXSUBFRAME > 40)
    {
        for (int layer = 0; layer < N_LAYER; layer++)
        {
            pd3dImmediateContext->ClearRenderTargetView(g_Ref_6_RenderingClass->ppRTVs[layer], ClearColor);
            pd3dImmediateContext->ClearDepthStencilView(g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0, 0);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_6_RenderingClass->ppRTVs[layer], NULL);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_6_RenderingClass->ppRTVs[layer], g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView());
            renderPerObjects(pd3dImmediateContext);

        }

        g_Ref_6_effectSRV_colorMap->SetResource(g_Ref_6_RenderingClass->ppSRVs[0]);
    }

    if (MAXSUBFRAME > 48)
    {
        for (int layer = 0; layer < N_LAYER; layer++)
        {
            pd3dImmediateContext->ClearRenderTargetView(g_Ref_7_RenderingClass->ppRTVs[layer], ClearColor);
            pd3dImmediateContext->ClearDepthStencilView(g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0, 0);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_7_RenderingClass->ppRTVs[layer], NULL);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_7_RenderingClass->ppRTVs[layer], g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView());
            renderPerObjects(pd3dImmediateContext);

        }

        g_Ref_7_effectSRV_colorMap->SetResource(g_Ref_7_RenderingClass->ppSRVs[0]);
    }

    if (MAXSUBFRAME > 56)
    {
        for (int layer = 0; layer < N_LAYER; layer++)
        {
            pd3dImmediateContext->ClearRenderTargetView(g_Ref_8_RenderingClass->ppRTVs[layer], ClearColor);
            pd3dImmediateContext->ClearDepthStencilView(g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0, 0);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_8_RenderingClass->ppRTVs[layer], NULL);
            pd3dImmediateContext->OMSetRenderTargets(1, &g_Ref_8_RenderingClass->ppRTVs[layer], g_Ref_pDepthBuffer_RenderingClass->GetDepthStencilView());
            renderPerObjects(pd3dImmediateContext);

        }

        g_Ref_8_effectSRV_colorMap->SetResource(g_Ref_8_RenderingClass->ppSRVs[0]);
    }







//-----------------------------------------------------------------------------
// 	   Reference: MakeMotionBlur
//-----------------------------------------------------------------------------

    ID3D11DepthStencilView* FQ_DSV = DXUTGetD3D11DepthStencilView();
    ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();

    pd3dImmediateContext->ClearRenderTargetView(pRTV, ClearColor);
    pd3dImmediateContext->ClearDepthStencilView(FQ_DSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
    pd3dImmediateContext->OMSetRenderTargets(1, &pRTV, FQ_DSV);

    g_pTech = g_pEffect->GetTechniqueByName("Ref_MotionBlur");
    g_pTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
    pd3dImmediateContext->Draw(3, 0);




    //---------------------------------------------------------------------------------

    // Tell the UI items to render 
    DXUT_BeginPerfEvent(DXUT_PERFEVENTCOLOR, L"HUD / Stats");
    g_HUD.OnRender(fElapsedTime);
    g_SampleUI.OnRender(fElapsedTime);
    RenderText();
    DXUT_EndPerfEvent();

    //---------------------------------------------------------------------------------
    //Print Texture
    //---------------------------------------------------------------------------------

    //static int frame = 0;
    //CString fileName = "motion blur";
    //if (frame >= 0 && frame <= 200) renderTagetToImageFile(pd3dDevice, pd3dImmediateContext, pRTV, frame, fileName);
    //frame++;



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
    LPCTSTR name = L"./Screenshots/shots/" + fileName + L".bmp";


    D3DX11SaveTextureToFile(pd3dImmediateContext, texture, D3DX11_IFF_BMP, name);
    texture->Release();
    backbuffer->Release();
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
    DXUTGetGlobalResourceCache().OnDestroyDevice();



}

