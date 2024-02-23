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

#include <atlstr.h> 
#include <string>

#include"ScreenGrab.h"
#include <d3dcompiler.h>

#include <iostream>

using namespace DirectX;
using namespace PackedVector;

#pragma warning( disable : 4100 )
#pragma warning( disable : 26482 )
#pragma warning( disable : 26429 )
#pragma warning( disable : 26481 )
#pragma warning( disable : 26485 )

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs

CFirstPersonCamera          g_Camera;
CD3DSettingsDlg             g_D3DSettingsDlg;       // Device settings dialog
CDXUTDialog                 g_HUD;                  // manages the 3D   
CDXUTDialog                 g_SampleUI;             // dialog for sample specific controls

// Resources
unique_ptr<CDXUTTextHelper> g_pTxtHelper;

ID3D10Blob* g_pEffectBlob;
ID3D10Blob* g_pErrorsBlob;

ID3D11InputLayout* g_pLayout;

//depth peeling
unique_ptr<RenderTexture> g_pColorMap_RenderingClass;
unique_ptr<RenderTexture> g_pMotionVectorMap_RenderingClass;
unique_ptr<RenderTexture> g_pMotionVectorSquareMap_RenderingClass;
unique_ptr<RenderTexture> g_pDepthBuffer_RenderingClass[2];
unique_ptr<RenderTexture> g_pMotionBlur_RenderingClass;
unique_ptr<RenderTexture> g_pDenoising_RenderingClass;
unique_ptr<RenderTexture> g_pPrintMotionBlurClass;

unique_ptr<RenderTexture> g_pNormalMap_RenderingClass;

ID3DX11EffectShaderResourceVariable* g_effectSRV_colorMap;
ID3DX11EffectShaderResourceVariable* g_effectSRV_motionVectorMap;
ID3DX11EffectShaderResourceVariable* g_effectSRV_motionVectorSquareMap;
ID3DX11EffectShaderResourceVariable* g_effectSRV_depthBuffer;
ID3DX11EffectShaderResourceVariable* g_effectSRV_motionBlur;
ID3DX11EffectShaderResourceVariable* g_effectSRV_denoising;

ID3DX11EffectShaderResourceVariable* g_effectSRV_normalMap;

//stencil buffer
unique_ptr<RenderTexture> g_color_Depth_motionVector_Class;
unique_ptr<RenderTexture> g_sorted_Color_Depth_MotionVector_Class;
unique_ptr<RenderTexture> g_motionVectorMipMap_Class;
unique_ptr<RenderTexture> g_motionVector_Class;
unique_ptr<RenderTexture> g_depth_Class;
unique_ptr<RenderTexture> g_firstLayerColor_Class;
unique_ptr<RenderTexture> g_MotionBlurColor_Class;
unique_ptr<RenderTexture> g_MotionBlurDepthWeight_Class;

ID3DX11EffectShaderResourceVariable* g_color_Depth_motionVectorMap[1];
ID3DX11EffectShaderResourceVariable* g_sorted_Color_Depth_MotionVectorMap;
ID3DX11EffectShaderResourceVariable* g_motionVectorMipMap;
ID3DX11EffectShaderResourceVariable* g_firstLayerColorMap;
ID3DX11EffectShaderResourceVariable* g_MotionBlurColorMap;
ID3DX11EffectShaderResourceVariable* g_MotionBlurDepthWeightMap;
ID3DX11EffectShaderResourceVariable* g_motionVectorMap;
ID3DX11EffectShaderResourceVariable* g_depthMap;

//ModelClass 
unique_ptr<RenderObject> g_pModel_1;
unique_ptr<RenderObject> g_pModel_2;
unique_ptr<RenderObject> g_pModel_3;
unique_ptr<RenderObject> g_pModel_4;
unique_ptr<RenderObject> g_pBackgroundModel;
unique_ptr<RenderObject> g_pFSQ;

unique_ptr<RenderObject> g_pSkybox;

bool g_isMove = true;
bool boxDrawing = false;


ID3DX11Effect* g_pDepthPeelingEffect;
ID3DX11Effect* g_pStencilBufferEffect;
ID3DX11EffectTechnique* g_pDepthPeelingTech;
ID3DX11EffectTechnique* g_pStencilBufferTech;
D3DX11_TECHNIQUE_DESC techDesc;

enum E_RENDERING_TYPES
{
    RENDERING_DEPTHPELLING,
    RENDERING_STENCILBUFFER
};

//E_RENDERING_TYPES g_iRendering = RENDERING_DEPTHPELLING;
E_RENDERING_TYPES g_iRendering = RENDERING_STENCILBUFFER;

#define IDC_RENDERING_GROUP             1
#define IDC_RENDERING_DEPTHPELLING      2
#define IDC_RENDERING_STENCILBUFFER     3


XMVECTORF32 tempVecEye = { 31.0f, -26.f, 18.f, 0.f };

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
# pragma region Forward declarations
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext) noexcept;
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext);
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext) noexcept;
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext) noexcept;
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo* AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo* DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext) noexcept;
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext);
void CALLBACK OnD3D11DestroyDevice(void* pUserContext);
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext);
void InitApp();

void renderTagetToImageFile(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11RenderTargetView* renderTaget, int frmae, CString fileName);
void BeginStackRender(ID3D11DeviceContext* pd3dDeviceContext, ID3D11RenderTargetView** ppRTVs, RenderTexture* RenderClass, UINT nRTV, ID3DX11EffectTechnique*);

void LoadModels(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
void RenderText();
void DepthPeeling_SetResouce();
void StencilBuffer_SetResouce();

void DepthPeeling(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
void StencilBuffer(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);

void renderPerObjects(ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech, ID3DX11Effect* pEffect);
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
    DXUTCreateWindow(L"motionBlur");
    DXUTCreateDevice(D3D_FEATURE_LEVEL_11_0, true, SCREENWIDTH, SCREENHEIGHT);
    DXUTMainLoop(); // Enter into the DXUT render loop

    return DXUTGetExitCode();
}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    g_D3DSettingsDlg.Init(&g_DialogResourceManager);
    g_HUD.Init(&g_DialogResourceManager);
    g_HUD.SetCallback(OnGUIEvent);
    g_SampleUI.Init(&g_DialogResourceManager);
    g_SampleUI.SetCallback(OnGUIEvent);
    g_Camera.SetRotateButtons(true, false, false);

    int iY = -770;
    g_SampleUI.AddRadioButton(IDC_RENDERING_DEPTHPELLING, IDC_RENDERING_GROUP, L"depth pelling", 0, iY += 26, 170, 22);
    g_SampleUI.AddRadioButton(IDC_RENDERING_STENCILBUFFER, IDC_RENDERING_GROUP, L"stencil buffer", 0, iY += 26, 170, 22);

    auto pRadioButton = g_SampleUI.GetRadioButton(IDC_RENDERING_STENCILBUFFER);
    //auto pRadioButton = g_SampleUI.GetRadioButton(IDC_RENDERING_DEPTHPELLING);
    pRadioButton->SetChecked(true);
}

//--------------------------------------------------------------------------------------
// Called right before creating a D3D device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext) noexcept
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
    g_pTxtHelper->SetForegroundColor(Colors::Red);
    g_pTxtHelper->DrawTextLine(DXUTGetFrameStats(DXUTIsVsyncEnabled()));
    g_pTxtHelper->DrawTextLine(DXUTGetDeviceStats());
    g_pTxtHelper->End();
}

//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext)
{
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
    if (*pbNoFurtherProcessing)
        return 0;

    if (g_D3DSettingsDlg.IsActive())
    {
        g_D3DSettingsDlg.MsgProc(hWnd, uMsg, wParam, lParam);
        return 0;
    }

    *pbNoFurtherProcessing = g_SampleUI.MsgProc(hWnd, uMsg, wParam, lParam);
    if (*pbNoFurtherProcessing)
        return 0;

    g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);
    return 0;

}

//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext) noexcept
{
    if (bKeyDown)
    {
        switch (nChar)
        {
        case 'P':
            g_isMove = true;
            break;

        case 'O':
            g_isMove = false;
            break;
        default:
            break;
        }
    }
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext) noexcept
{
    switch (nControlID)
    {
    case IDC_RENDERING_DEPTHPELLING:
        g_iRendering = RENDERING_DEPTHPELLING;
        break;
    case IDC_RENDERING_STENCILBUFFER:
        g_iRendering = RENDERING_STENCILBUFFER;
        break;
    default:
        break;
    }
}

//--------------------------------------------------------------------------------------
// Reject any D3D devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo* AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo* DeviceInfo,
    DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext) noexcept
{
    return true;
}

# pragma region Load Models and texture
void LoadModels(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
    g_pModel_1 = make_unique<RenderObject>();
    g_pModel_2 = make_unique<RenderObject>();
    g_pModel_3 = make_unique<RenderObject>();     
    g_pModel_4 = make_unique<RenderObject>();
    g_pBackgroundModel = make_unique<RenderObject>();
    g_pFSQ = make_unique<RenderObject>();
    g_pSkybox = make_unique<RenderObject>();

    g_pFSQ->MakeLoadModel("./Objects/FSQ.obj", pd3dDevice, pd3dImmediateContext);
    //g_pSkybox->MakeLoadModel("./Objects/skybox/skybox_sphere.obj", pd3dDevice, pd3dImmediateContext);

    g_pModel_1->MakeLoadModel("./Objects/cars/car_1.obj", pd3dDevice, pd3dImmediateContext);
    g_pModel_2->MakeLoadModel("./Objects/cars/car_2.obj", pd3dDevice, pd3dImmediateContext);
    g_pModel_3->MakeLoadModel("./Objects/cars/car_3.obj", pd3dDevice, pd3dImmediateContext);
    g_pModel_4->MakeLoadModel("./Objects/cars/car_4.obj", pd3dDevice, pd3dImmediateContext);
    g_pBackgroundModel->MakeLoadModel("./Objects/city/123123444.obj", pd3dDevice, pd3dImmediateContext);

    //g_pBackgroundModel->MakeLoadModel("./Objects/sponza_1/sponza.obj", pd3dDevice, pd3dImmediateContext);
    //g_pBackgroundModel->MakeLoadModel("./Objects/nanosuit/nanosuit.obj", pd3dDevice, pd3dImmediateContext);
    //g_pBackgroundModel->MakeLoadModel("./Objects/Sponza/sponza.obj", pd3dDevice, pd3dImmediateContext);
    //g_pBackgroundModel->MakeLoadModel("./Objects/San_Miguel/san-miguel-low-poly.obj", pd3dDevice, pd3dImmediateContext);
    //g_pBackgroundModel->MakeLoadModel("./Objects/dragon.obj", pd3dDevice, pd3dImmediateContext);
    //g_pBackgroundModel->MakeLoadModel("./Objects/living_room/living_room.obj", pd3dDevice, pd3dImmediateContext);
}
# pragma endregion

bool isOne = true;

//--------------------------------------------------------------------------------------
// Create any D3D resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
    HRESULT hr = S_OK;

    ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
    V_RETURN(g_DialogResourceManager.OnD3D11CreateDevice(pd3dDevice, pd3dImmediateContext));
    V_RETURN(g_D3DSettingsDlg.OnD3D11CreateDevice(pd3dDevice));
    g_pTxtHelper = make_unique<CDXUTTextHelper>(pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15);

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
    dwShaderFlags |= D3DCOMPILE_DEBUG;
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

    //-----------------------------------------------------------------------------------
    //---------------------------depth peeling Initialize-------------------------------
    //-----------------------------------------------------------------------------------

    hr = D3DX11CompileFromFile(L"DepthPeelingShader.fx", 0, 0, 0, "fx_5_0", dwShaderFlags, 0, 0, &g_pEffectBlob, &g_pErrorsBlob, 0);

    if (g_pErrorsBlob)
    {
        MessageBoxA(0, (char*)g_pErrorsBlob->GetBufferPointer(), 0, 0);
        g_pErrorsBlob->Release();
    }

    hr = D3DX11CreateEffectFromMemory(g_pEffectBlob->GetBufferPointer(), g_pEffectBlob->GetBufferSize(), 0, pd3dDevice, &g_pDepthPeelingEffect);
    assert(SUCCEEDED(hr));
    g_pEffectBlob->Release();

    g_pColorMap_RenderingClass = make_unique<RenderTexture>();
    g_pColorMap_RenderingClass->InitializeBackBufferWithMipMap_Array(pd3dDevice, N_LAYER);

    g_pMotionVectorMap_RenderingClass = make_unique<RenderTexture>();
    g_pMotionVectorMap_RenderingClass->InitializeBackBufferWithMipMap_Array(pd3dDevice, N_LAYER);

    g_pMotionVectorSquareMap_RenderingClass = make_unique<RenderTexture>();
    g_pMotionVectorSquareMap_RenderingClass->InitializeBackBufferWithMipMap_Array(pd3dDevice, N_LAYER);

    g_pNormalMap_RenderingClass = make_unique<RenderTexture>();
    g_pNormalMap_RenderingClass->InitializeBackBufferWithMipMap_Array(pd3dDevice, N_LAYER);

    for (int i = 0; i < 2; i++)
    {
        g_pDepthBuffer_RenderingClass[i] = make_unique<RenderTexture>();
        g_pDepthBuffer_RenderingClass[i].get()->Initialize_DepthBuffer_DepthPeeling(pd3dDevice);
    }

    g_pMotionBlur_RenderingClass = make_unique<RenderTexture>();
    g_pMotionBlur_RenderingClass->Initialize(pd3dDevice);

    g_pDenoising_RenderingClass = make_unique<RenderTexture>();
    g_pDenoising_RenderingClass->Initialize(pd3dDevice);

    g_pPrintMotionBlurClass = make_unique<RenderTexture>();
    g_pPrintMotionBlurClass->Initialize(pd3dDevice);

    DepthPeeling_SetResouce();

    //-----------------------------------------------------------------------------------
    //----------------------------stencil buffer Initialize------------------------------
    //-----------------------------------------------------------------------------------

    hr = D3DX11CompileFromFile(L"StencilBufferShader.fx", 0, 0, 0, "fx_5_0", dwShaderFlags, 0, 0, &g_pEffectBlob, &g_pErrorsBlob, 0);

    if (g_pErrorsBlob)
    {
        MessageBoxA(0, (char*)g_pErrorsBlob->GetBufferPointer(), 0, 0);
        g_pErrorsBlob->Release();
    }

    hr = D3DX11CreateEffectFromMemory(g_pEffectBlob->GetBufferPointer(), g_pEffectBlob->GetBufferSize(), 0, pd3dDevice, &g_pStencilBufferEffect);
    assert(SUCCEEDED(hr));
    g_pEffectBlob->Release();

    g_color_Depth_motionVector_Class = make_unique<RenderTexture>();
    g_color_Depth_motionVector_Class->Initialize(pd3dDevice, RENDER_TARGET_MSAA);

    g_sorted_Color_Depth_MotionVector_Class = make_unique<RenderTexture>();
    g_sorted_Color_Depth_MotionVector_Class->Initialize(pd3dDevice, RENDER_TARGET_ARRAY);

    g_motionVectorMipMap_Class = make_unique<RenderTexture>();
    g_motionVectorMipMap_Class->Initialize(pd3dDevice, RENDER_TARGET_MIPMAP);

    g_firstLayerColor_Class = make_unique<RenderTexture>();
    g_firstLayerColor_Class->Initialize(pd3dDevice, RENDER_TARGET);

    g_depth_Class = make_unique<RenderTexture>();
    g_depth_Class.get()->Initialize(pd3dDevice, RENDER_TARGET);

    g_motionVector_Class = make_unique<RenderTexture>();
    g_motionVector_Class->Initialize(pd3dDevice, RENDER_TARGET);

    g_MotionBlurColor_Class = make_unique<RenderTexture>();
    g_MotionBlurColor_Class->Initialize(pd3dDevice, RENDER_TARGET);

    g_MotionBlurDepthWeight_Class = make_unique<RenderTexture>();
    g_MotionBlurDepthWeight_Class->Initialize(pd3dDevice, RENDER_TARGET);

    StencilBuffer_SetResouce();

    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------

    D3DX11_PASS_DESC passDesc;
    g_pStencilBufferTech = g_pStencilBufferEffect->GetTechniqueByName("Rendering");
    g_pStencilBufferTech->GetPassByIndex(0)->GetDesc(&passDesc);

    //const D3D11_INPUT_ELEMENT_DESC layout[] =
    //{
    //    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
    //    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
    //    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0  },
    //};

    const D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0  },
    };


    V_RETURN(pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &g_pLayout));

    LoadModels(pd3dDevice, pd3dImmediateContext);
    g_pModel_4->xPos = 100;

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

    constexpr float fAspectRatio = SCREENWIDTH / SCREENHEIGHT;
    g_Camera.SetProjParams(XM_PI / 4, fAspectRatio, ZNEAR, ZFAR);

    g_HUD.SetLocation(SCREENWIDTH - 170, 0);
    g_HUD.SetSize(170, 170);

    g_SampleUI.SetLocation(SCREENWIDTH - 170, SCREENHEIGHT - 300);
    g_SampleUI.SetSize(170, 300);
    return S_OK;
}

float perTime = 0.f;

void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext)
{
    if (g_D3DSettingsDlg.IsActive())
    {
        g_D3DSettingsDlg.OnRender(fElapsedTime);
        return;
    }

    //static float degree = 0;

    //XMVECTOR a;
    //a.m128_f32[0] = g_Camera.GetLookAtPt().m128_f32[0];
    //a.m128_f32[1] = g_Camera.GetLookAtPt().m128_f32[1];
    //a.m128_f32[2] = g_Camera.GetLookAtPt().m128_f32[2];
    //a.m128_f32[3] = g_Camera.GetLookAtPt().m128_f32[3];

   
    //a = XMVector3Transform(a, XMMatrixRotationY(degree * 3.141592 / 180));
    //const XMVECTORF32 vecEye = { 5.0f, -26.f, 16.f, 0.f };

    //g_Camera.SetViewParams(vecEye, a);

    //degree += 90 * fElapsedTime;
    //perTime = fElapsedTime;

    ////if (degree > 1)
    ////{
    ////    degree = 0;
    //// }


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

    pd3dImmediateContext->IASetInputLayout(NULL);

    if (g_isMove)
    {
        g_pModel_1->xPos = g_pModel_1->SetMovePosition(g_pModel_1->xPos, 1.5f, 50.f, -50.f);
        g_pModel_2->xPos = g_pModel_2->SetMovePosition(g_pModel_2->xPos, 1.5f, 50.f, -50.f);
        g_pModel_3->xPos = g_pModel_3->SetMovePosition(g_pModel_3->xPos, 1.5f, 50.f, -50.f);
        g_pModel_4->xPos = g_pModel_4->SetMovePosition(g_pModel_4->xPos, 1.5f, -50.f, 50.f);

        //g_pModel_1->xPos = g_pModel_1->SetMovePosition(g_pModel_1->xPos, 0.f, 50.f, -50.f);
        //g_pModel_2->xPos = g_pModel_2->SetMovePosition(g_pModel_2->xPos, 0.f, 50.f, -50.f);
        //g_pModel_3->xPos = g_pModel_3->SetMovePosition(g_pModel_3->xPos, 0.f, 50.f, -50.f);
        //g_pModel_4->xPos = g_pModel_4->SetMovePosition(g_pModel_4->xPos, 0.f, -50.f, 50.f);
    }

    g_pModel_1->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 0.02f, -g_pModel_1->xPos, -0.0f, -0.f);
    g_pModel_2->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 0.02f, -g_pModel_2->xPos, -0.0f, -0.f);
    g_pModel_3->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 0.02f, -g_pModel_3->xPos, -0.0f, -0.f);
    g_pModel_4->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 0.02f, -g_pModel_4->xPos, -0.0f, -0.f);
    g_pBackgroundModel->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 0.02f, 0.0f, 0.0f, 0.f);
    //g_pBackgroundModel->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 0.1f, 5.0f, -56.f, 16.f);
    //g_pBackgroundModel->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 0.2f, 10.0f, -20.0f, 0.f);

    //g_pSkybox->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 80.0f, 0.0f, 0.0f, 0.f);
    g_pSkybox->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 80.0f, g_Camera.GetEyePt().m128_f32[0], g_Camera.GetEyePt().m128_f32[1] - 20.1f , g_Camera.GetEyePt().m128_f32[2]);

    switch (g_iRendering) {
    case RENDERING_DEPTHPELLING:
        DepthPeeling(pd3dDevice, pd3dImmediateContext);
        break;

    case RENDERING_STENCILBUFFER:
        StencilBuffer(pd3dDevice, pd3dImmediateContext);
        break;
    }

    g_pModel_1->AfterFrameRender();
    g_pModel_2->AfterFrameRender();
    g_pModel_3->AfterFrameRender();
    g_pModel_4->AfterFrameRender();
    g_pBackgroundModel->AfterFrameRender();
    g_pSkybox->AfterFrameRender();

//---------------------------------------------------------------------------------

    DXUT_BeginPerfEvent(DXUT_PERFEVENTCOLOR, L"HUD / Stats");
    g_HUD.OnRender(fElapsedTime);
    g_SampleUI.OnRender(fElapsedTime);
    RenderText();
    DXUT_EndPerfEvent();


}

void DepthPeeling(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{

    //-----------------------------------------------------------------------------
    // 	   1PASS:	Clear Texture
    //-----------------------------------------------------------------------------

    g_pDepthPeelingTech = g_pDepthPeelingEffect->GetTechniqueByName("ClearTexture");
    g_pDepthPeelingTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);

    pd3dImmediateContext->OMSetRenderTargets(N_LAYER, g_pColorMap_RenderingClass->ppRTVs, NULL);
    g_pFSQ->RenderFSQ(pd3dImmediateContext, g_pLayout, g_pDepthPeelingTech, techDesc, g_pDepthPeelingEffect);

    pd3dImmediateContext->OMSetRenderTargets(N_LAYER, g_pMotionVectorMap_RenderingClass->ppRTVs, NULL);
    g_pFSQ->RenderFSQ(pd3dImmediateContext, g_pLayout, g_pDepthPeelingTech, techDesc, g_pDepthPeelingEffect);

    pd3dImmediateContext->OMSetRenderTargets(1, g_pMotionVectorSquareMap_RenderingClass->ppRTVs, NULL);
    g_pFSQ->RenderFSQ(pd3dImmediateContext, g_pLayout, g_pDepthPeelingTech, techDesc, g_pDepthPeelingEffect);


    pd3dImmediateContext->OMSetRenderTargets(N_LAYER, g_pNormalMap_RenderingClass->ppRTVs, NULL);
    g_pFSQ->RenderFSQ(pd3dImmediateContext, g_pLayout, g_pDepthPeelingTech, techDesc, g_pDepthPeelingEffect);

    //-----------------------------------------------------------------------------
    // 	   2PASS:	DepthPeeling Render
    //-----------------------------------------------------------------------------

    //g_pSkybox->BeforeFrameRender(g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix(), 10.0f, 0.0f, 0.0f, 0.f);
    //g_pSkybox->Render(pd3dImmediateContext, g_pLayout, g_pDepthPeelingTech, techDesc, g_pDepthPeelingEffect);

    ID3D11ShaderResourceView* DepthBuffer_SRVs[] =
    {
        g_pDepthBuffer_RenderingClass[0]->GetShaderResourceView(),
        g_pDepthBuffer_RenderingClass[1]->GetShaderResourceView()
    };

    ID3D11DepthStencilView* DepthBuffer_DSVs[] =
    {
        g_pDepthBuffer_RenderingClass[0]->GetDepthStencilView(),
        g_pDepthBuffer_RenderingClass[1]->GetDepthStencilView()
    };

    pd3dImmediateContext->ClearDepthStencilView(DepthBuffer_DSVs[1], D3D11_CLEAR_DEPTH, 0.0, 0);

    for (int layer = 0; layer < N_LAYER; layer++)
    {
        int currId = layer % 2;
        int prevId = 1 - currId;

        if (layer == 0)
        {
            ID3D11RenderTargetView* Layers_RTVs[] =
            {
                g_pColorMap_RenderingClass->ppRTVs[layer],
                g_pMotionVectorMap_RenderingClass->ppRTVs[layer],
                g_pMotionVectorSquareMap_RenderingClass->ppRTVs[layer],
                g_pNormalMap_RenderingClass ->ppRTVs[layer]
            };

            pd3dImmediateContext->ClearDepthStencilView(DepthBuffer_DSVs[currId], D3D11_CLEAR_DEPTH, 1.0, 0);
            //pd3dImmediateContext->OMSetRenderTargets(3, Layers_RTVs, NULL);

            pd3dImmediateContext->OMSetRenderTargets(4, Layers_RTVs, NULL);

            //g_pDepthPeelingTech = g_pDepthPeelingEffect->GetTechniqueByName("DepthPeelingRendering");
            g_pDepthPeelingTech = g_pDepthPeelingEffect->GetTechniqueByName("DepthPeelingRenderingWithSkybox");
            g_pDepthPeelingTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
            g_effectSRV_depthBuffer->SetResource(DepthBuffer_SRVs[prevId]);
            //pd3dImmediateContext->OMSetRenderTargets(3, Layers_RTVs, DepthBuffer_DSVs[currId]);

            pd3dImmediateContext->OMSetRenderTargets(4, Layers_RTVs, DepthBuffer_DSVs[currId]);

            boxDrawing = true;
            renderPerObjects(pd3dImmediateContext, g_pDepthPeelingTech, g_pDepthPeelingEffect);

        }

        else
        {
            ID3D11RenderTargetView* Layers_RTVs[] =
            {
                g_pColorMap_RenderingClass->ppRTVs[layer],
                g_pMotionVectorMap_RenderingClass->ppRTVs[layer],
                g_pNormalMap_RenderingClass->ppRTVs[layer]
            };

            pd3dImmediateContext->ClearDepthStencilView(DepthBuffer_DSVs[currId], D3D11_CLEAR_DEPTH, 1.0, 0);
            //pd3dImmediateContext->OMSetRenderTargets(2, Layers_RTVs, NULL);
            pd3dImmediateContext->OMSetRenderTargets(3, Layers_RTVs, NULL);

            g_pDepthPeelingTech = g_pDepthPeelingEffect->GetTechniqueByName("DepthPeelingRendering");
            g_pDepthPeelingTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
            g_effectSRV_depthBuffer->SetResource(DepthBuffer_SRVs[prevId]);

            //pd3dImmediateContext->OMSetRenderTargets(2, Layers_RTVs, DepthBuffer_DSVs[currId]);
            pd3dImmediateContext->OMSetRenderTargets(3, Layers_RTVs, DepthBuffer_DSVs[currId]);
            renderPerObjects(pd3dImmediateContext, g_pDepthPeelingTech, g_pDepthPeelingEffect);
        }
    }

    pd3dImmediateContext->GenerateMips(g_pMotionVectorMap_RenderingClass->ppSRVs[0]);
    pd3dImmediateContext->GenerateMips(g_pMotionVectorSquareMap_RenderingClass->ppSRVs[0]);

    //---------------------------------------------------------------------------------
    //PASS 3 Make MotionBlur on FullSceenQuad
    //---------------------------------------------------------------------------------

    ID3D11RenderTargetView* Layers_RTVs = g_pPrintMotionBlurClass->GetRenderTargetView();

    pd3dImmediateContext->OMSetRenderTargets(1, &Layers_RTVs, NULL);
    g_pDepthPeelingTech = g_pDepthPeelingEffect->GetTechniqueByName("MotionBlur");
    g_pDepthPeelingTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
    g_pFSQ->RenderFSQ(pd3dImmediateContext, g_pLayout, g_pDepthPeelingTech, techDesc, g_pDepthPeelingEffect);

    //---------------------------------------------------------------------------------
    //PASS 4 Denoising
    //---------------------------------------------------------------------------------

    ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
    pd3dImmediateContext->OMSetRenderTargets(1, &pRTV, NULL);
    g_pDepthPeelingTech = g_pDepthPeelingEffect->GetTechniqueByName("Denoising");
    g_pDepthPeelingTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
    g_pFSQ->RenderFSQ(pd3dImmediateContext, g_pLayout, g_pDepthPeelingTech, techDesc, g_pDepthPeelingEffect);

    //static int frame = 0;
    //CString fileName = "motion blur";
    //if (frame >= 1 && frame <= 100) renderTagetToImageFile(pd3dDevice, pd3dImmediateContext, pRTV, frame, fileName);
    //frame++;


}

void StencilBuffer(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext) {
    const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    ID3D11RenderTargetView* pMSAARTV = g_color_Depth_motionVector_Class->GetRenderTargetView();
    pd3dImmediateContext->OMSetRenderTargets(1, &pMSAARTV, NULL);

    g_pStencilBufferTech = g_pStencilBufferEffect->GetTechniqueByName("TextureClear");
    g_pStencilBufferTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
    pd3dImmediateContext->Draw(3, 0);

    //------------------------------------------------------------- stencil buffer clear
    g_pStencilBufferTech = g_pStencilBufferEffect->GetTechniqueByName("StencilBufferClear");
    g_pStencilBufferTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
    BeginStackRender(pd3dImmediateContext, &pMSAARTV, g_color_Depth_motionVector_Class.get(), 1, g_pStencilBufferTech);

    //------------------------------------------------------------- rendering (stencil buffer use)

    g_pStencilBufferTech = g_pStencilBufferEffect->GetTechniqueByName("Rendering");
    g_pStencilBufferTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
    renderPerObjects(pd3dImmediateContext, g_pStencilBufferTech, g_pStencilBufferEffect);

    //------------------------------------------------------------- rendering

    g_pStencilBufferTech = g_pStencilBufferEffect->GetTechniqueByName("motionVectorMipMap_colorMap_Rendering");
    pd3dImmediateContext->OMSetDepthStencilState(g_motionVectorMipMap_Class->GetUseDepthTestDSS(), 1);

    ID3D11RenderTargetView* pmotionVectorMomentMapRTV[4];
    pmotionVectorMomentMapRTV[0] = g_motionVectorMipMap_Class->GetRenderTargetView();
    pmotionVectorMomentMapRTV[1] = g_firstLayerColor_Class->GetRenderTargetView();
    pmotionVectorMomentMapRTV[2] = g_motionVector_Class->GetRenderTargetView();
    pmotionVectorMomentMapRTV[3] = g_depth_Class.get()->GetRenderTargetView();;

    ID3D11DepthStencilView* pmotionVectorMomentMapDSV = g_motionVectorMipMap_Class->GetDepthStencilView();

    pd3dImmediateContext->ClearRenderTargetView(pmotionVectorMomentMapRTV[0], ClearColor);
    pd3dImmediateContext->ClearRenderTargetView(pmotionVectorMomentMapRTV[1], ClearColor);
    pd3dImmediateContext->ClearRenderTargetView(pmotionVectorMomentMapRTV[2], ClearColor);
    pd3dImmediateContext->ClearRenderTargetView(pmotionVectorMomentMapRTV[3], ClearColor);

    pd3dImmediateContext->ClearDepthStencilView(pmotionVectorMomentMapDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    pd3dImmediateContext->OMSetRenderTargets(4, pmotionVectorMomentMapRTV, pmotionVectorMomentMapDSV);

    g_pStencilBufferTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
    boxDrawing = true;
    renderPerObjects(pd3dImmediateContext, g_pStencilBufferTech, g_pStencilBufferEffect);

    pd3dImmediateContext->GenerateMips(g_motionVectorMipMap_Class->GetShaderResourceView());

    //------------------------------------------------------------- sorting

    g_pStencilBufferTech = g_pStencilBufferEffect->GetTechniqueByName("Sorting");
    ID3D11RenderTargetView* pSortedRTV[MSAA_LEVEL] = {};
    for (int i = 0; i < MSAA_LEVEL; i++) pSortedRTV[i] = g_sorted_Color_Depth_MotionVector_Class->GetRenderTargetViewArray(i);

    pd3dImmediateContext->OMSetRenderTargets(MSAA_LEVEL, pSortedRTV, NULL);

    g_pStencilBufferTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
    pd3dImmediateContext->Draw(3, 0);

    //------------------------------------------------------------- motion blur

    g_pStencilBufferTech = g_pStencilBufferEffect->GetTechniqueByName("MakeMotionBlur");
    ID3D11RenderTargetView* pMotionVectorRTV[2] = {};

    pMotionVectorRTV[0] = g_MotionBlurColor_Class->GetRenderTargetView();
    pMotionVectorRTV[1] = g_MotionBlurDepthWeight_Class->GetRenderTargetView();

    pd3dImmediateContext->OMSetRenderTargets(2, pMotionVectorRTV, NULL);

    g_pStencilBufferTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
    pd3dImmediateContext->Draw(3, 0);

    //------------------------------------------------------------- denosing

    g_pStencilBufferTech = g_pStencilBufferEffect->GetTechniqueByName("Denoising");
    g_pStencilBufferTech = g_pStencilBufferEffect->GetTechniqueByName("DenoisingWithReliability");
    ID3D11RenderTargetView* pDXUTRTV = DXUTGetD3D11RenderTargetView();

    pd3dImmediateContext->OMSetRenderTargets(1, &pDXUTRTV, NULL);

    g_pStencilBufferTech->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
    pd3dImmediateContext->Draw(3, 0);




//---------------------------------------------------------------------------------
//Print Texture
//---------------------------------------------------------------------------------

    //static int frame = 0;
    //CString fileName = "motion blur";
    //if (frame >= 1 && frame <= 100) renderTagetToImageFile(pd3dDevice, pd3dImmediateContext, pDXUTRTV, frame, fileName);
    //frame++;


    //static float a = 17;

    //const XMVECTORF32 vecEye = { 5.0f, -26.f, 16.f, 0.f };
    //const XMVECTORF32 vecAt = { a + frame * 20, -26.f, 0.f, 0.f };
    //g_Camera.SetViewParams(vecEye, vecAt);
    //frame++;

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

    SAFE_RELEASE(g_pLayout);
    SAFE_RELEASE(g_pDepthPeelingEffect);
}

void BeginStackRender(ID3D11DeviceContext* pd3dDeviceContext, ID3D11RenderTargetView** ppRTVs, RenderTexture* RenderClass, UINT nRTV, ID3DX11EffectTechnique* tech)
{
    tech->GetPassByIndex(0)->Apply(0, pd3dDeviceContext);

    pd3dDeviceContext->IASetInputLayout(NULL);
    pd3dDeviceContext->RSSetState(RenderClass->GetRasterizerState());

    pd3dDeviceContext->OMSetRenderTargets(0, NULL, RenderClass->GetDepthStencilView());

    UINT8 nStencilRef = MSAA_LEVEL;

    const FLOAT blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    UINT nSampleMask = 1;

    pd3dDeviceContext->ClearDepthStencilView(RenderClass->GetDepthStencilView(), D3D11_CLEAR_STENCIL, 1.0f, nStencilRef);
    for (UINT8 i = 1; i < MSAA_LEVEL; ++i)
    {
        --nStencilRef;
        nSampleMask = nSampleMask << 1;

        pd3dDeviceContext->OMSetBlendState(RenderClass->GetBlendState(), blendFactor, nSampleMask);
        pd3dDeviceContext->OMSetDepthStencilState(RenderClass->GetWriteDepthStencilState(), nStencilRef);
        pd3dDeviceContext->Draw(3, 0);
    }

    pd3dDeviceContext->OMSetRenderTargets(nRTV, ppRTVs, RenderClass->GetDepthStencilView());
    pd3dDeviceContext->OMSetDepthStencilState(RenderClass->GetUseDepthStencilState(), 0x00000001);
}

void StencilBuffer_SetResouce() {
    g_color_Depth_motionVectorMap[0] = g_pStencilBufferEffect->GetVariableByName("g_color_Depth_motionVectorMap")->AsShaderResource();
    g_sorted_Color_Depth_MotionVectorMap = g_pStencilBufferEffect->GetVariableByName("g_sorted_Color_Depth_MotionVectorMap")->AsShaderResource();
    g_motionVectorMipMap = g_pStencilBufferEffect->GetVariableByName("g_motionVectorMipMap")->AsShaderResource();
    g_firstLayerColorMap = g_pStencilBufferEffect->GetVariableByName("g_firstLayerColorMap")->AsShaderResource();
    g_motionVectorMap = g_pStencilBufferEffect->GetVariableByName("g_motionVectorMap")->AsShaderResource();
    g_depthMap = g_pStencilBufferEffect->GetVariableByName("g_depthMap")->AsShaderResource();
    g_MotionBlurColorMap = g_pStencilBufferEffect->GetVariableByName("g_MotionBlurColorMap")->AsShaderResource();
    g_MotionBlurDepthWeightMap = g_pStencilBufferEffect->GetVariableByName("g_MotionBlurDepthWeightMap")->AsShaderResource();

    g_color_Depth_motionVectorMap[0]->SetResource(g_color_Depth_motionVector_Class->GetShaderResourceView());
    g_sorted_Color_Depth_MotionVectorMap->SetResource(g_sorted_Color_Depth_MotionVector_Class->GetShaderResourceViewArray(0));
    g_motionVectorMipMap->SetResource(g_motionVectorMipMap_Class->GetShaderResourceView());
    g_firstLayerColorMap->SetResource(g_firstLayerColor_Class->GetShaderResourceView());
    g_motionVectorMap->SetResource(g_motionVector_Class->GetShaderResourceView());
    g_MotionBlurColorMap->SetResource(g_MotionBlurColor_Class->GetShaderResourceView());
    g_MotionBlurDepthWeightMap->SetResource(g_MotionBlurDepthWeight_Class->GetShaderResourceView());
    g_depthMap->SetResource(g_depth_Class.get()->GetShaderResourceView());
}

void DepthPeeling_SetResouce()
{
    g_effectSRV_colorMap              = g_pDepthPeelingEffect->GetVariableByName("g_colorMap")->AsShaderResource();
    g_effectSRV_motionVectorMap       = g_pDepthPeelingEffect->GetVariableByName("g_motionVectorMap")->AsShaderResource();
    g_effectSRV_motionVectorSquareMap = g_pDepthPeelingEffect->GetVariableByName("g_motionVectorSquareMap")->AsShaderResource();
    g_effectSRV_depthBuffer           = g_pDepthPeelingEffect->GetVariableByName("g_depthBuffer")->AsShaderResource();
    g_effectSRV_motionBlur            = g_pDepthPeelingEffect->GetVariableByName("g_motionBlur")->AsShaderResource();
    g_effectSRV_denoising             = g_pDepthPeelingEffect->GetVariableByName("g_denosing")->AsShaderResource();

    g_effectSRV_colorMap->SetResource(g_pColorMap_RenderingClass->ppSRVs[0]);
    g_effectSRV_motionVectorMap->SetResource(g_pMotionVectorMap_RenderingClass->ppSRVs[0]);
    g_effectSRV_motionVectorSquareMap->SetResource(g_pMotionVectorSquareMap_RenderingClass->ppSRVs[0]);
    g_effectSRV_motionBlur->SetResource(g_pPrintMotionBlurClass->GetShaderResourceView());
    g_effectSRV_denoising->SetResource(g_pDenoising_RenderingClass->GetShaderResourceView());


    g_effectSRV_normalMap = g_pDepthPeelingEffect->GetVariableByName("g_normalMap")->AsShaderResource();
    g_effectSRV_normalMap->SetResource(g_pNormalMap_RenderingClass->ppSRVs[0]);
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

void renderPerObjects(ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech, ID3DX11Effect* pEffect)
{

    if (boxDrawing)
    {
        g_pSkybox->Render(pd3dImmediateContext, g_pLayout, pTech, techDesc, pEffect);
        boxDrawing = false;
    }

    g_pModel_1->Render(pd3dImmediateContext, g_pLayout, pTech, techDesc, pEffect);
    g_pModel_2->Render(pd3dImmediateContext, g_pLayout, pTech, techDesc, pEffect);
    g_pModel_3->Render(pd3dImmediateContext, g_pLayout, pTech, techDesc, pEffect);
    g_pModel_4->Render(pd3dImmediateContext, g_pLayout, pTech, techDesc, pEffect);
    g_pBackgroundModel->Render(pd3dImmediateContext, g_pLayout, pTech, techDesc, pEffect);

}