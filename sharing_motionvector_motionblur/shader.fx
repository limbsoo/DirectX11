//-----------------------------------------------------------------------------
// File: shader.fx
// The HLSL file for the DDSWithoutD3DX sample for the Direct3D 11 device
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

# include "LightHelper.fx"

cbuffer cbPerObject
{
	matrix             g_mWorldViewProjection;
	matrix			   g_mPreviousWorldViewProjection; //previous only this  ,other all current//g_mPreviousWorldViewProjection
	matrix			   g_mMVPInverseTransposed;
	matrix             g_mWorldView;
	matrix             g_mWorld;
};
// Textures and Samplers
Texture2D           g_txDiffuse;

Texture2D			g_ColorMap;
Texture2D			g_motionVector3DMap;
Texture2D			g_motionVector3DSquareMap;
Texture2D			g_previousDepthMap;

Texture2D			g_ColorMap1;
Texture2D			g_motionVector3DMap1;
Texture2D			g_previousDepthMap1;

Texture2D			g_ColorMap2;
Texture2D			g_motionVector3DMap2;
Texture2D			g_previousDepthMap2;

Texture2D			g_ColorMap3;
Texture2D			g_motionVector3DMap3;
Texture2D			g_previousDepthMap3;

Texture2D			g_ColorMap4;
Texture2D			g_motionVector3DMap4;
Texture2D			g_previousDepthMap4;

Texture2D			g_ColorMap5;
Texture2D			g_motionVector3DMap5;
Texture2D			g_previousDepthMap5;

Texture2D			g_ColorMap6;
Texture2D			g_motionVector3DMap6;
Texture2D			g_previousDepthMap6;

Texture2D			g_FullscreenQuad;
Texture2D			g_FullscreenQuadDenoising;

Texture2D			g_Scene;

//Texture2DMS<float4, MSAA_LEVEL> g_TestMSAAScene;
Texture2DMS<float4> g_MSAAScene;
#if KBUFFER_PASSES > 1
Texture2DMS<float4, MSAA_LEVEL> g_MSAAScene1;
#endif


SamplerState        g_samLinear;

float gWidth = 1920.0f;//screen width size
float gHeight = 1080.0f;//screen height size
static const int randomPickNum = 12; // random pick number in motion boundary
float thresholdX = 0.001f;	
float thresholdY = 0.001f;
int maxSearchCount = 3;
int samplingTime = 10;
static const float eps = 0.001f;

SamplerState		g_samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
	AddressW = Clamp;
};

RasterizerState rasterizerState_New
{
	FillMode = SOLID;
	CullMode = NONE;
};

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct VS_INPUT
{
	float4 Position    : POSITION; // vertex position 	
	float3 Normal	   : NORMAL;
	float2 TextureUV   : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position     : SV_POSITION; // vertex position //저장할 위치
	float4 prePosition  : POSITION0;
	float4 curPosition  : POSITION1;
	float3 Normal		: NORMAL;
	float2 TexCoord     : TEXCOORD0;   // vertex texture coords 
};

float Linearize(float depth, float near, float far)
{
	return (far * near) / (far - depth * (far - near));
}

float lengthSquare(float2 a) {
	return (a.x * a.x) + (a.y * a.y);
}

float3 homogenious2uv(float4 pos)
{
	float3 position3D = float3(0.0f, 0.0f, 0.0f);

	pos /= pos.w;
	pos.xy += float2(1, 1);
	pos.xy /= float2(2, 2);
	pos.z = Linearize(pos.z, 0.1f, 1000.0f);
	position3D = float3(pos.x, pos.y, pos.z);

	return position3D;
}

struct OutputPS
{
	float4 color;
	float4 motionVector3D;
	float4 motionVector3DSquare;
	float4 previousDepth;
};

VS_OUTPUT RenderScenePreviousVS(VS_INPUT InputVS)
{

	VS_OUTPUT OutputVS = (VS_OUTPUT)0;

	OutputVS.Position = mul(InputVS.Position, g_mPreviousWorldViewProjection);//처음엔 0
	OutputVS.prePosition = OutputVS.Position; //이전위치
	OutputVS.curPosition = mul(InputVS.Position, g_mWorldViewProjection);//현재위치

	OutputVS.TexCoord = InputVS.TextureUV;
	OutputVS.Normal = InputVS.Normal;

	return OutputVS;
}

OutputPS RenderScenePreviousPS(VS_OUTPUT InputPS) : SV_TARGET
{
	float3 currentPos = homogenious2uv(InputPS.curPosition); //현재위치
	float3 previousPos = homogenious2uv(InputPS.prePosition); //이전위치
	float3 motionVector = currentPos - previousPos;

	float3 motionVectorSquare = float3(motionVector.x * motionVector.x, motionVector.y * motionVector.y, motionVector.z * motionVector.z);
	float4 color = g_txDiffuse.SampleLevel(g_samLinear, InputPS.TexCoord, 0);

	float depth = Linearize(InputPS.Position.z, 0.1f, 1000.0f); 

	OutputPS outputPS;

	outputPS.color = color;
	outputPS.motionVector3D = float4(motionVector, 1.0f);
	outputPS.motionVector3DSquare = float4(motionVectorSquare, 1.0f);
	outputPS.previousDepth = float4(depth, depth, depth, 1.0f);

	return outputPS;
}

technique11 RenderScene_Previous
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, RenderScenePreviousVS()));
		SetPixelShader(CompileShader(ps_5_0, RenderScenePreviousPS()));
		SetRasterizerState(rasterizerState_New);
	}
}

//////////////////////////////////////////////////////////// 2PASS ////////////////////////////////////////////////////////////////////////
float Get_Random(float2 uv)
{
	return frac(sin(dot(uv, float2(12.9898f, 78.233f) * 2.0f)) * 43758.5453f);
}
//0~1사이 값

float2 rand_2_10(float2 uv) {
	float noiseX = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
	float noiseY = sqrt(1 - noiseX * noiseX);
	return float2(noiseX, noiseY);
}

float4 motionBoundary(float2 uv, float mipmapLevelForGaussian)
{
	const int constant = 3;
												
	float2 mean = g_motionVector3DMap.SampleLevel(g_samLinear, uv, mipmapLevelForGaussian).xy;
	//mean = g_motionVector3DMap.SampleLevel(g_samLinear, pixelPosition-mean, mipmapLevelForGaussian).xy;
	float2 squaMean = g_motionVector3DSquareMap.SampleLevel(g_samLinear, uv, mipmapLevelForGaussian).xy;

	float2 standardDeviation = sqrt(squaMean - mean * mean);//제곱의 평균 -평균의 제곱 : 표준편차 제곱 =>분산
	float2 Max = mean + standardDeviation * constant;
	float2 Min = mean - standardDeviation * constant;

	float2 leftTopCorner = max(uv - Max, float2(0.0f, 0.0f));
	float2 rightBottomCorner = min(uv - Min, float2(1.0f, 1.0f));

	return float4 (leftTopCorner, rightBottomCorner);
}

float2 currentPixelLine(float2 uv, float2 lineStart, float2 lineMotionVector) {

	float2 tempUV = uv;

	for (int i = 0; i < maxSearchCount; i++) {

		uv = tempUV - lineStart;//원점 방향으로 이동된 uv
		float changedTargetPositionX = uv.x * lineMotionVector.x + uv.y * lineMotionVector.y;//x 축으로 회전
		float changedTargetPositionY = uv.x * (-lineMotionVector.y) + uv.y * lineMotionVector.x;
		float rangeOfX = (lineMotionVector.x * lineMotionVector.x) + (lineMotionVector.y * lineMotionVector.y) + thresholdX; // a2 + b2 + threshold
		float rangeOfY = thresholdY;
		float currentPixelDepth = g_previousDepthMap.SampleLevel(g_samPoint, lineStart, 0).x;
		float currentPixelMotionVectorZ = g_motionVector3DMap.SampleLevel(g_samPoint, lineStart, 0).z;
		float Y = currentPixelDepth + (changedTargetPositionX * currentPixelMotionVectorZ / rangeOfX);
		float Z = currentPixelDepth / Y;

		if (changedTargetPositionX <= rangeOfX && changedTargetPositionX >= -thresholdX && changedTargetPositionY <= rangeOfY * Z && changedTargetPositionY >= -thresholdY * Z) {
			return lineMotionVector;
		}

		lineStart = tempUV - lineMotionVector * (1.0f / randomPickNum);
		lineMotionVector = g_motionVector3DMap.SampleLevel(g_samPoint, lineStart, 0).xy;
	}
	return float2(-2.0f, -2.0f);
}

float4 findLineAndGetColor(float2 uv)
{
	float4 resultColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float2 UV_1 = float2(0.0f, 0.0f);
	float2 UV_2 = float2(0.0f, 0.0f);
	float randomx = 0.0f;
	float randomy = 0.0f;
	float2 warpStart = float2(0.0f, 0.0f);
	int lineIndexPre = 0;
	float2 linePre[10];

	float4 searchBoundary = motionBoundary(uv, 8.0f);
	for (int j = 0; j < randomPickNum; j++)
	{
		bool isFind = false;
		UV_1 = float2(uv.x + j * 0.0001357f, uv.y + j * 0.0004567f);
		UV_2 = float2(uv.x + j * 0.0002468f, uv.y + j * 0.0003456f);
		randomx = Get_Random(UV_1); //random -> return 0~0.999999
		randomy = Get_Random(UV_2);

		warpStart = float2(searchBoundary.x + randomx * abs(searchBoundary.z - searchBoundary.x),
			searchBoundary.y + randomy * abs(searchBoundary.w - searchBoundary.y));//motion boundary 내의 random한 점

		float2 lineMotionVector = g_motionVector3DMap.SampleLevel(g_samPoint, warpStart, 0).xy;
		float2 foundLine = currentPixelLine(uv, warpStart, lineMotionVector);
		if (lengthSquare(foundLine) + eps < 8.0f) {
			for (int searchingIndex = 0; searchingIndex < lineIndexPre; searchingIndex++) {
				if (abs(foundLine.x - linePre[searchingIndex].x) < eps && abs(foundLine.y - linePre[searchingIndex].y) < eps) {
					isFind = true;
					break;
				}
			}
			if (!isFind) {
				linePre[lineIndexPre] = foundLine;
				lineIndexPre++;
			}
		}
	}

	float currentPixelDepth = 0.0f;
	float prevPixelDepth = 0.0f;
	int nNumOfSucess = 0;
	float3 sumColor = float3(0.0f, 0.0f, 0.0f);
	float3 sumDepth = float3(0.0f, 0.0f, 0.0f);
	float targetDepth;

	float3 targetMotionVector;
	float3 targetStaticColor;
	float targetStaticDepth;

	targetMotionVector = g_motionVector3DMap.SampleLevel(g_samPoint, uv, 0).xyz;
	targetStaticColor = g_ColorMap.SampleLevel(g_samPoint, uv, 0).xyz;
	targetStaticDepth = g_previousDepthMap.SampleLevel(g_samPoint, uv, 0).x;

	if (!(targetMotionVector.x == 0.0f && targetMotionVector.y == 0.0f)) {
		targetMotionVector = g_motionVector3DMap1.SampleLevel(g_samPoint, uv, 0).xyz;
		targetStaticColor = g_ColorMap1.SampleLevel(g_samPoint, uv, 0).xyz;
		targetStaticDepth = g_previousDepthMap1.SampleLevel(g_samPoint, uv, 0).x;
	}
	if (!(targetMotionVector.x == 0.0f && targetMotionVector.y == 0.0f)) {
		targetMotionVector = g_motionVector3DMap2.SampleLevel(g_samPoint, uv, 0).xyz;
		targetStaticColor = g_ColorMap2.SampleLevel(g_samPoint, uv, 0).xyz;
		targetStaticDepth = g_previousDepthMap2.SampleLevel(g_samPoint, uv, 0).x;
	}
	if (!(targetMotionVector.x == 0.0f && targetMotionVector.y == 0.0f)) {
		targetMotionVector = g_motionVector3DMap3.SampleLevel(g_samPoint, uv, 0).xyz;
		targetStaticColor = g_ColorMap3.SampleLevel(g_samPoint, uv, 0).xyz;
		targetStaticDepth = g_previousDepthMap3.SampleLevel(g_samPoint, uv, 0).x;
	}
	if (!(targetMotionVector.x == 0.0f && targetMotionVector.y == 0.0f)) {
		targetMotionVector = g_motionVector3DMap4.SampleLevel(g_samPoint, uv, 0).xyz;
		targetStaticColor = g_ColorMap4.SampleLevel(g_samPoint, uv, 0).xyz;
		targetStaticDepth = g_previousDepthMap4.SampleLevel(g_samPoint, uv, 0).x;
	}
	if (!(targetMotionVector.x == 0.0f && targetMotionVector.y == 0.0f)) {
		targetMotionVector = g_motionVector3DMap5.SampleLevel(g_samPoint, uv, 0).xyz;
		targetStaticColor = g_ColorMap5.SampleLevel(g_samPoint, uv, 0).xyz;
		targetStaticDepth = g_previousDepthMap5.SampleLevel(g_samPoint, uv, 0).x;
	}
	if (!(targetMotionVector.x == 0.0f && targetMotionVector.y == 0.0f)) {
		targetMotionVector = g_motionVector3DMap6.SampleLevel(g_samPoint, uv, 0).xyz;
		targetStaticColor = g_ColorMap6.SampleLevel(g_samPoint, uv, 0).xyz;
		targetStaticDepth = g_previousDepthMap6.SampleLevel(g_samPoint, uv, 0).x;
	}
	
	float2 randomNumber = rand_2_10(uv);
	for (int k = 0; k < samplingTime; k++) {//시간에서 탐색
		randomNumber = rand_2_10(randomNumber);
		float3 targetColor = float3(-1.0f, 0.0f, 0.0f);
		targetDepth = 1000.0f;
		
		for (int p = 0; p < lineIndexPre; p++) {// 저장된 모션 벡터 각각을 현재 시간에 대해 탐색
			float2 prevPixelPos = uv - linePre[p] * (k+ randomNumber) / samplingTime;
			float3 prevPixelMotionVector = g_motionVector3DMap.SampleLevel(g_samPoint, prevPixelPos, 0).xyz;
			prevPixelDepth = g_previousDepthMap.SampleLevel(g_samPoint, prevPixelPos, 0).x;

			if (distance(linePre[p], prevPixelMotionVector.xy) < 0.0025f) {

				prevPixelMotionVector.z *= (float)(k+ randomNumber) / samplingTime;

				if (prevPixelDepth + prevPixelMotionVector.z <= targetDepth) {

					targetDepth = prevPixelDepth + prevPixelMotionVector.z;
					targetColor = g_ColorMap.SampleLevel(g_samPoint, prevPixelPos, 0).xyz;

				}
			}
		}
		if (targetMotionVector.x == 0.0f && targetMotionVector.y == 0.0f && targetStaticDepth < targetDepth) {
			targetDepth = targetStaticDepth;
			targetColor = targetStaticColor;
		}
		if (targetColor.r >= 0.0f) {
			sumColor += targetColor;
			sumDepth += float3(targetDepth, targetDepth, targetDepth);
			nNumOfSucess++;
		}
	}
	
	float3 bluredColor = float3(0, 0, 0);
	if (nNumOfSucess > 0)
		bluredColor = sumColor / nNumOfSucess;
	float weightOfSuccessRate = (float)nNumOfSucess / samplingTime;
	return float4(bluredColor, weightOfSuccessRate);
}

struct FullScreenQuadVertexOut
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

FullScreenQuadVertexOut FullScreenQuadVS(uint vertexID : SV_VertexID) //vertexID = 꼭지점 개수
{
	FullScreenQuadVertexOut OutputVS;
	//---------------------------------------------------------------------------------
	// Option 1: Cull Mode = Back face culling
	//---------------------------------------------------------------------------------
	OutputVS.uv = float2((vertexID << 1) & 2, vertexID & 2);
	OutputVS.position = float4(OutputVS.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	//---------------------------------------------------------------------------------
	// Option 2: Cull Mode = Front face culling
	//---------------------------------------------------------------------------------
	//OutputVS.uv = float2(((vertexID << 1) & 2) * 2.0f, (vertexID == 0) * -4.0f);
	//OutputVS.position = float4(OutputVS.uv + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	return OutputVS;
}

float4 FullScreenQuadPS(FullScreenQuadVertexOut In) : SV_TARGET
{
	//float4 depth = g_previousDepthMap.SampleLevel(g_samLinear, In.uv, 0);

	return findLineAndGetColor(In.uv);
}

technique11 RenderScene_FullScreenQuad
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
		SetPixelShader(CompileShader(ps_5_0, FullScreenQuadPS()));
		SetRasterizerState(rasterizerState_New);
	}
}

//-------------------------------------3Pass denoising----------------

static const float Gaussiankernel[25] =
{
   0.00390625f, 0.015625f,   0.0234375f,   0.015625f,   0.00390625f,
   0.015625f,    0.0625f,   0.09375f,   0.0625f,   0.015625f,
   0.0234375f,    0.09375f,   0.140625f,   0.09375f,   0.0234375f,
   0.015625f,    0.0625f,   0.09375f,   0.0625f,   0.015625f,
   0.00390625f, 0.015625f,   0.0234375f,   0.015625f,   0.00390625f,
};

static const float2 offsets[25] =
{
   float2(-2.0f, -2.0f), float2(-1.0f, -2.0f), float2(0.0f, -2.0f), float2(1.0f, -2.0f), float2(2.0f, -2.0f),
   float2(-2.0f, -1.0f), float2(-1.0f, -1.0f), float2(0.0f, -1.0f), float2(1.0f, -1.0f), float2(2.0f, -2.0f),
   float2(-2.0f, 0.0f),  float2(-1.0f, 0.0f),  float2(0.0f, 0.0f),  float2(1.0f, 0.0f),  float2(2.0f, 0.0f),
   float2(-2.0f, 1.0f),  float2(-1.0f, 1.0f),  float2(0.0f, 1.0f),  float2(1.0f, 1.0f),  float2(2.0f, 1.0f),
   float2(-2.0f, 2.0f),  float2(-1.0f, 2.0f),  float2(0.0f, 2.0f),  float2(1.0f, 2.0f),  float2(2.0f, 2.0f),
};


float4 doEdgeAvoidATrousWavelet_improved(float ScreenWidth, float ScreenHeight, float2 uv, float stepwidth)
{
	float weight, l_w, cum_w = 0.0f;
	float2 UV, deltaDepth, deltaLuminance, step;
	float3 ntmp, RGBtmp, sum, generalColor;
	float searchSuccessRatetmp = 0.0f;

	generalColor = g_ColorMap.Sample(g_samPoint, uv).xyz;
	step = float2(1.0f / ScreenWidth, 1.0f / ScreenHeight);
	sum = float3(0.0f, 0.0f, 0.0f);

	[unroll(25)] for (int j = 0; j < 25; j++)
	{
		UV = uv + offsets[j] * step * stepwidth;

		// Luminance
		RGBtmp = g_FullscreenQuad.Sample(g_samPoint, UV).xyz;
		// Luminance
		searchSuccessRatetmp = g_FullscreenQuad.Sample(g_samPoint, UV).w;

		float3 t = generalColor - RGBtmp.xyz;
		float c_w = max(min(1.0 - dot(t, t), 1.0), 0.0);

		l_w = Gaussiankernel[j] * searchSuccessRatetmp;// *c_w;
		//l_w = searchSuccessRatetmp;
		//l_w = Gaussiankernel[j];

		weight = l_w;
		sum += RGBtmp * weight;
		cum_w += weight;
	}

	sum /= cum_w;

	//return float4(cum_w/25, cum_w / 25, cum_w / 25, 1.0f);
	return float4(sum, 1.0f);
}
//-------------------------------------denoising----------------

struct FullScreenQuadDenoisingVertexOut
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

FullScreenQuadDenoisingVertexOut FullScreenQuadDenoisingVS(uint vertexID : SV_VertexID)
{
	FullScreenQuadDenoisingVertexOut OutputVS;
	//---------------------------------------------------------------------------------
	// Option 1: Cull Mode = Back face culling
	//---------------------------------------------------------------------------------
	OutputVS.uv = float2((vertexID << 1) & 2, vertexID & 2);
	OutputVS.position = float4(OutputVS.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	//---------------------------------------------------------------------------------
	// Option 2: Cull Mode = Front face culling
	//---------------------------------------------------------------------------------
	//OutputVS.uv = float2(((vertexID << 1) & 2) * 2.0f, (vertexID == 0) * -4.0f);
	//OutputVS.position = float4(OutputVS.uv + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	return OutputVS;
}

float4 FullScreenQuadDenoisingPS(FullScreenQuadDenoisingVertexOut In) : SV_TARGET
{
	float stepWidth = 1.0f;

	float4 color = doEdgeAvoidATrousWavelet_improved(gWidth, gHeight, In.uv, stepWidth);// 디노이징 한거
	//float4 color = g_FullscreenQuad.Sample(g_samLinear, In.uv);// 디노이징 하기전
	//float4 color = g_ColorMap.SampleLevel(g_samLinear, In.uv, 0);
	//float4 color = g_motionVector3DMap4.SampleLevel(g_samLinear, In.uv, 0);
	//float4 color = g_previousDepthMap.SampleLevel(g_samLinear, In.uv, 0)/100;

	return color;
}

technique11 RenderScene_FullScreenQuad_Denoising
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, FullScreenQuadDenoisingVS()));
		SetPixelShader(CompileShader(ps_5_0, FullScreenQuadDenoisingPS()));
		SetRasterizerState(rasterizerState_New);
	}

}

/////////////////////////test///////////////////////////////////
struct firstSceneOutPutVS
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

firstSceneOutPutVS firstVS(VS_INPUT Input)
{
	firstSceneOutPutVS OutputVS;

	OutputVS.position = mul(Input.Position, g_mWorldViewProjection);
	//OutputVS.position = Input.Position;
	OutputVS.uv = Input.TextureUV;

	return OutputVS;
}

float4 firstPS(firstSceneOutPutVS input) : SV_TARGET
{
	return g_txDiffuse.Sample(g_samPoint, input.uv);
//return float4(1, 1, 1, 1);
}

technique11 firstScene
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, firstVS()));
		SetPixelShader(CompileShader(ps_5_0, firstPS()));
		SetRasterizerState(rasterizerState_New);
	}

}
/////////////////////////test///////////////////////////////////


struct TestMSAAOut
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

TestMSAAOut MSAAVS(uint vertexID : SV_VertexID)
{
	TestMSAAOut OutputVS;
	OutputVS.uv = float2((vertexID << 1) & 2, vertexID & 2);
	OutputVS.position = float4(OutputVS.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	return OutputVS;
}

float4 MSAAPS(TestMSAAOut input) : SV_TARGET
{
	float4 returncol = float4(0, 0, 0, 0);
	for (uint i = 0; i < MSAA_LEVEL; i++)
	{
		returncol += g_MSAAScene.Load(input.uv * float2(screenWidth, screenHeight), i);
		//returncol += float4(0.0, 1.0, 0, 1.0);
	}

	//return g_Scene.Sample(g_samPoint, input.uv);
	return returncol / MSAA_LEVEL;
}

technique11 MSAAScene
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, MSAAVS()));
		SetPixelShader(CompileShader(ps_5_0, MSAAPS()));
		SetRasterizerState(rasterizerState_New);
	}
}