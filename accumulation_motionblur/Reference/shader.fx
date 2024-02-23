//-----------------------------------------------------------------------------
// File: shader.fx
// The HLSL file for the DDSWithoutD3DX sample for the Direct3D 11 device
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "LightHelper.fx"
#include "define.h"

matrix		g_FPS;

cbuffer cbPerFrame
{
	DirectionalLight   gDirLight;
	PointLight         gPointLight;
	SpotLight          gSpotLight;
	float4             gEyePosW;
	Material           gMaterial;
};

cbuffer cbPerObject
{
	matrix             g_mWorldViewProjection;
	matrix			   g_mPreviousWorldViewProjection; //previous only this  ,other all current//g_mPreviousWorldViewProjection
	matrix			   g_mMVPInverseTransposed;
	matrix             g_mWorldView;
	matrix             g_mWorld;
};

// Textures and Samplers
Texture2D				g_txDiffuse;

Texture2DArray<float4>	g_colorMap;
Texture2DArray<float4>	g_motionVectorMap;
Texture2DArray<float4>	g_motionVectorSquareMap;
Texture2D<float4>		g_depthBuffer;

Texture2D				g_motionBlur;
Texture2D				g_denosing;

SamplerState    g_samLinear;

SamplerState	g_samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
	AddressW = Clamp;
};

RasterizerState rasterizerState
{
	FillMode = SOLID;
};

RasterizerState rasterizerState_New
{
	FillMode = SOLID;
	CullMode = NONE;
};

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float  specularPower;
	float4 specularColor;
};


float Linearize(float depth, float near, float far)// z' -> z, z'은 lineraize하지 않기때문에 왜곡되기 전의 z가 필요하기 때문에 사용
{
	return (far * near) / (far - depth * (far - near));
	//return (2.0f * near) / (far + near - depth * (far - near));
}

float distanceSquare(float2 a, float2 b) {
	float distance = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
	return distance;
}

float lengthSquare(float2 a) {
	return (a.x * a.x) + (a.y * a.y);
}


float3 homogeneous2uv(float4 pos)//homogeneous-> depth를 lineraize 해, uv좌표 만드는거
{
	float3 position3D = float3(0.0f, 0.0f, 0.0f);

	// 0~1사이로 바꿈
	pos /= pos.w;
	pos.x *= 0.5f;
	pos.x += 0.5f;
	pos.y *= -0.5f;
	pos.y += 0.5f;
	pos.z = Linearize(pos.z, 0.1f, 1000.0f);

	position3D = float3(pos.x, pos.y, pos.z);

	return position3D;
}

BlendState NoMRT_BS
{
	RenderTargetWriteMask[0] = 0x0f;
};

BlendState MRT2_BS
{
	RenderTargetWriteMask[0] = 0x0f;
	RenderTargetWriteMask[1] = 0x0f;
};

BlendState MRT5_BS
{
	RenderTargetWriteMask[0] = 0x0f;
	RenderTargetWriteMask[1] = 0x0f;
	RenderTargetWriteMask[2] = 0x0f;
	RenderTargetWriteMask[3] = 0x0f;
	RenderTargetWriteMask[4] = 0x0f;
};

DepthStencilState DepthNoStencil_DS
{
	DepthEnable = true;
	DepthFunc = Less_Equal;
	DepthWriteMask = All;
	StencilEnable = false;
};

DepthStencilState NoDepthNoStencil_DS
{
	DepthEnable = false;
	StencilEnable = false;
};

RasterizerState NoCullNoMSAA_RS
{
	CullMode = None;
	FillMode = Solid;
	MultisampleEnable = false;
};

RasterizerState BackCullNoMSAA_RS
{
	CullMode = Back;
	FillMode = Solid;
	MultisampleEnable = false;
};


//-----------------------------------------------------------------------------
// 	   1PASS:	Clear Texture
//-----------------------------------------------------------------------------

struct QuadOutput
{
	float4 pos : SV_Position;
	float2 tex : TEXCOORD;
};

QuadOutput FullScreenTriVS(uint id : SV_VertexID) //꼭지점 INDEX 0,1,2
{
	QuadOutput output = (QuadOutput)0.0f;
	output.tex = float2((id << 1) & 2, id & 2);
	output.pos = float4(output.tex * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	return output;
}

struct DepthColorOutputMRT
{
	float4 DepthColor0 : SV_Target0;
	float4 DepthColor1 : SV_Target1;
	float4 DepthColor2 : SV_Target2;
	float4 DepthColor3 : SV_Target3;
	float4 DepthColor4 : SV_Target4;
	float4 DepthColor5 : SV_Target5;
	float4 DepthColor6 : SV_Target6;
	float4 DepthColor7 : SV_Target7;
};


DepthColorOutputMRT ClearPS(QuadOutput IN)//depth값 초기화, depth값은 작은 순서대로 그려지므로 MAX_DEPTH값으로 초기화
{
	DepthColorOutputMRT output;

	output.DepthColor0 = float4(0, 0, 0, MAX_DEPTH);
	output.DepthColor1 = float4(0, 0, 0, MAX_DEPTH);
	output.DepthColor2 = float4(0, 0, 0, MAX_DEPTH);
	output.DepthColor3 = float4(0, 0, 0, MAX_DEPTH);
	output.DepthColor4 = float4(0, 0, 0, MAX_DEPTH);
	output.DepthColor5 = float4(0, 0, 0, MAX_DEPTH);
	output.DepthColor6 = float4(0, 0, 0, MAX_DEPTH);
	output.DepthColor7 = float4(0, 0, 0, MAX_DEPTH);

	return output;
}

technique11 ClearTexture
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, FullScreenTriVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ClearPS()));
		SetRasterizerState(NoCullNoMSAA_RS);
		SetDepthStencilState(NoDepthNoStencil_DS, 0x00000000);
		SetBlendState(MRT2_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
	}
}

technique11 ClearDepthStencilBuffer
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, FullScreenTriVS()));
		SetGeometryShader(NULL);
		SetPixelShader(NULL);
	}
}


//-----------------------------------------------------------------------------
// 	   2PASS:	DepthPeeling Render
//-----------------------------------------------------------------------------

float4 DepthPeelingPostPS(QuadOutput IN) : SV_TARGET
{
	float4 color = float4(0.0f, 0.0f, 0.0f,0.0f);
	float4 frag[N_MAX];

	for (int i = 0; i < MSAA_LEVEL; ++i)
	{
		//frag[0] = g_ColorMap.SampleLevel(g_samLinear,IN.tex, 0);

		frag[i] = g_colorMap.Load(int4(IN.tex * float2(SCREENWIDTH, SCREENHEIGHT), i, 0));
		//frag[i] = g_motionVectorMap.Load(int4(IN.tex * float2(screenWidth, screenHeight), i, 0));
		//frag[i] = g_motionVectorSquareMap.Load(int4(IN.tex * float2(screenWidth, screenHeight), i, 0));
	}

	color = float4(frag[0].xyz,0);

	return color;
}


technique11 LayersDrawOnFullscreenquad
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, FullScreenTriVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, DepthPeelingPostPS()));

		//SetRasterizerState(NoCullNoMSAA_RS);
		SetRasterizerState(rasterizerState_New);

		//SetDepthStencilState(DepthNoStencil_DS, 0x00000000);
		//SetDepthStencilState(NoDepthNoStencil_DS, 0x00000000);
		//SetBlendState(MRT5_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
	}
}


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

struct RenderOutput_PS
{
	float4 color;
	float4 motionVector3D;
	float4 motionVector3DSquare;
};

VS_OUTPUT DepthPeelingRendering_VS(VS_INPUT InputVS)
{

	VS_OUTPUT OutputVS = (VS_OUTPUT)0;

	OutputVS.Position = mul(InputVS.Position, g_mPreviousWorldViewProjection);//처음엔 0
	OutputVS.prePosition = OutputVS.Position;
	OutputVS.curPosition = mul(InputVS.Position, g_mWorldViewProjection);//현재위치

	OutputVS.TexCoord = InputVS.TextureUV;
	OutputVS.Normal = InputVS.Normal;

	return OutputVS;
}

RenderOutput_PS DepthPeelingRendering_PS(VS_OUTPUT InputPS) : SV_TARGET
{
	float3 currentPos = homogeneous2uv(InputPS.curPosition);
	float3 previousPos = homogeneous2uv(InputPS.prePosition);
	float3 motionVector = currentPos - previousPos;

	//motionVector *= -1;

	//motionVector *= 2;

	//motionVector += 1;

	//if (motionVector.x < 0 && motionVector.y < 0)
	//{
	//	motionVector = -motionVector;
	//}

	float3 motionVectorSquare = float3(motionVector.x * motionVector.x, motionVector.y * motionVector.y, motionVector.z * motionVector.z);
	float4 color = g_txDiffuse.Sample(g_samLinear, InputPS.TexCoord, 0);
	float depth = Linearize(InputPS.Position.z, 0.1f, 1000.0f);

	RenderOutput_PS outputPS;

	outputPS.color = float4(color.xyz, depth);
	outputPS.motionVector3D = float4(motionVector, 1.0f);
	outputPS.motionVector3DSquare = float4(motionVectorSquare, 1.0f);

	float zFront = g_depthBuffer.Load(int3(InputPS.Position.xy, 0)).r;
	if (InputPS.Position.z <= zFront) discard;

	return outputPS;
}

technique11 DepthPeelingRendering
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, DepthPeelingRendering_VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, DepthPeelingRendering_PS()));

		//SetRasterizerState(rasterizerState_New);
		SetRasterizerState(BackCullNoMSAA_RS);
		SetDepthStencilState(DepthNoStencil_DS, 0x00000000);
		SetBlendState(NoMRT_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
	}
}


//---------------------------------------------------------------------------------
//PASS 3 Make MotionBlur on FullSceenQuad
//---------------------------------------------------------------------------------


//---------------------------------------------------------------------------------
// Input: It uses texture coords as the random number seed.
// Output: Random number: [0,1), that is between 0.0 and 0.999999... inclusive.
// Author: Michael Pohoreski, Copyright: Copyleft 2012 :-)
// https://stackoverflow.com/questions/5149544/can-i-generate-a-random-number-inside-a-pixel-shader
//---------------------------------------------------------------------------------
float Get_Random(float2 uv)//
{
	return frac(sin(dot(uv, float2(12.9898f, 78.233f) * 2.0f)) * 43758.5453f);
}
float2 rand_2_10(in float2 uv) {
	float noiseX = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
	float noiseY = sqrt(1 - noiseX * noiseX);
	return float2(noiseX, noiseY);
}


float4 motionBoundary(float2 uv, float mipmapLevelForGaussian)
{
	const int constant = 3;

	float2 mean = g_motionVectorMap.SampleLevel(g_samLinear, float3(uv, 0), mipmapLevelForGaussian).xy;
	float2 squaMean = g_motionVectorSquareMap.SampleLevel(g_samLinear, float3(uv, 0), mipmapLevelForGaussian).xy;

	float2 standardDeviation = sqrt(squaMean - mean * mean);//제곱의 평균 -평균의 제곱 : 표준편차 제곱 =>분산
	float2 Max = mean + standardDeviation * constant;
	float2 Min = mean - standardDeviation * constant;

	float2 leftTopCorner = max(uv - Max, float2(0.0f, 0.0f));
	float2 rightBottomCorner = min(uv - Min, float2(1.0f, 1.0f));

	return float4 (leftTopCorner, rightBottomCorner);
}

bool IsIntersect(float2 uv, float2 lineStart, float2 lineMotionVector)
{
	float thresholdX = 0.001f;
	float thresholdY = 0.001f;

	float2 MovingSearchPosition = uv;
	uv = MovingSearchPosition - lineStart;//원점 방향으로 이동된 uv

	float changedTargetPositionX = uv.x * lineMotionVector.x + uv.y * lineMotionVector.y;//x 축으로 회전
	float changedTargetPositionY = uv.x * (-lineMotionVector.y) + uv.y * lineMotionVector.x;

	// a2 + b2 + threshold
	float rangeOfX = (lineMotionVector.x * lineMotionVector.x) + (lineMotionVector.y * lineMotionVector.y) + thresholdX;
	float rangeOfY = thresholdY;

	float currentPixelDepth = g_colorMap.SampleLevel(g_samLinear, float3(lineStart, 0), 0).w;
	float currentPixelMotionVectorZ = g_motionVectorMap.SampleLevel(g_samLinear, float3(lineStart, 0), 0).z;

	float Y = currentPixelDepth + (changedTargetPositionX * currentPixelMotionVectorZ / rangeOfX);
	float Z = currentPixelDepth / Y;

	if (changedTargetPositionX <= rangeOfX && changedTargetPositionX >= -thresholdX &&
		changedTargetPositionY <= rangeOfY * Z && changedTargetPositionY >= -thresholdY * Z)
	{
		return true;
	}

	else
	{
		return false;
	}


}

//float4 findLineAndGetColor(float2 uv)
//{
//	static const float eps = 0.001f;
//
//	float4 resultColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
//	float2 UV_1 = float2(0.0f, 0.0f);
//	float2 UV_2 = float2(0.0f, 0.0f);
//	float randomx = 0.0f;
//	float randomy = 0.0f;
//
//	int lineIndexPre = 0;
//	float2 linePre[10];
//
//	bool bIntersecting = false;
//	float2 StartingPoint = float2(0.0f, 0.0f);
//	float2 StartingPointMotionVector;
//	int intersectingCount;
//	float2 intersectedMotionVector[10];
//
//	float2 intersectedMotionV1111ector[10];
//	float4 searchBoundary = motionBoundary(uv, 8.0f);
//
//	for (int i = 0; i < RANDOMPICKCOUNT; i++)
//	{
//
//		UV_1 = float2(uv.x + i * 0.0001357f, uv.y + i * 0.0004567f);
//		UV_2 = float2(uv.x + i * 0.0002468f, uv.y + i * 0.0003456f);
//
//		randomx = Get_Random(UV_1);
//		randomy = Get_Random(UV_2);
//
//		StartingPoint = float2(searchBoundary.x + randomx * abs(searchBoundary.z - searchBoundary.x),
//							   searchBoundary.y + randomy * abs(searchBoundary.w - searchBoundary.y));
//
//		//StartingPointMotionVector = g_motionVectorMap.SampleLevel(g_samLinear, float3(StartingPoint, 0), 0).xy;
//		StartingPointMotionVector = g_motionVectorMap.Load(int4(StartingPoint * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0)).xy;
//
//		for (int j = 0; j < SEARCHCOUNT; j++)
//		{
//			bIntersecting = IsIntersect(uv, StartingPoint, StartingPointMotionVector);
//
//			if (bIntersecting == true)
//			{
//				break;
//			}
//
//			else
//			{
//				StartingPoint = uv - StartingPointMotionVector * (1.0f / RANDOMPICKCOUNT);
//				//StartingPointMotionVector = g_motionVectorMap.SampleLevel(g_samLinear, float3(StartingPoint, 0), 0).xy;
//				StartingPointMotionVector = g_motionVectorMap.Load(int4(StartingPoint * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0)).xy;
//			}
//
//		}
//
//		if (bIntersecting == true)
//		{
//			for (int k = 0; k < intersectingCount; k++)
//			{
//				if (abs(StartingPointMotionVector.x - intersectedMotionVector[k].x) < eps &&
//					abs(StartingPointMotionVector.y - intersectedMotionVector[k].y) < eps)
//				{
//					break;
//				}
//			}
//
//			if (k == intersectingCount)
//			{
//				intersectedMotionVector[intersectingCount] = StartingPointMotionVector;
//				intersectingCount++;
//			}
//		}
//	}
//
//	float currentPixelDepth = 0.0f;
//	float BackwardSampled_Depth = 0.0f;
//	float2 BackwardSampled_PixelPosition;
//	float3 BackwardSampled_MotionVector;
//
//	int SucessCount = 0;
//
//	float3 sumColor = float3(0.0f, 0.0f, 0.0f);
//	float3 sumDepth = float3(0.0f, 0.0f, 0.0f);
//	float targetDepth;
//	float3 targetColor;
//
//	float2 randomNumber = rand_2_10(uv);
//
//	for (int samplingIndex = 0; samplingIndex < SAMPLINGTIME; samplingIndex++) //시간에서 탐색
//	{
//		randomNumber = rand_2_10(randomNumber);
//
//		targetColor = float3(-1.0f, 0.0f, 0.0f);
//		targetDepth = 1000.0f;
//
//		for (int j = 0; j < intersectingCount; j++)
//		{
//			for (int k = 0; k < LAYERSCOUNT; k++) // 저장된 모션 벡터 각각을 현재 시간에 대해 탐색
//			{
//				BackwardSampled_PixelPosition = uv - intersectedMotionVector[j] * ((samplingIndex + randomNumber) / SAMPLINGTIME);
//				BackwardSampled_MotionVector = g_motionVectorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), k, 0)).xyz;
//				BackwardSampled_Depth = g_colorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), k, 0)).w;
//
//				if (distance(intersectedMotionVector[j], BackwardSampled_MotionVector.xy) < 0.0025f)
//				{
//					BackwardSampled_MotionVector.z *= (float)(samplingIndex + randomNumber) / SAMPLINGTIME;
//
//					if (BackwardSampled_Depth + BackwardSampled_MotionVector.z <= targetDepth)
//					{
//						targetDepth = BackwardSampled_Depth + BackwardSampled_MotionVector.z;
//						targetColor = g_colorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), k, 0)).xyz;
//						break;
//
//					}
//				}
//			}
//		}
//
//		if (targetColor.r >= 0.0f)
//		{
//			sumColor += targetColor;
//			SucessCount++;
//		}
//	}
//
//	float3 bluredColor = float3(0, 0, 0);
//
//	if (SucessCount > 0)
//	{
//		bluredColor = sumColor / SucessCount;
//	}
//
//	float SuccessRate = (float)SucessCount / SAMPLINGTIME;
//
//	return float4(bluredColor, SuccessRate);
//}

float4 findLineAndGetColor(float2 uv)
{
	static const float eps = 0.001f;

	float4 resultColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float2 UV_1 = float2(0.0f, 0.0f);
	float2 UV_2 = float2(0.0f, 0.0f);
	float randomx = 0.0f;
	float randomy = 0.0f;

	bool bIntersecting = false;
	float2 StartingPoint = float2(0.0f, 0.0f);
	float2 StartingPointMotionVector;

	int intersectingCount[N_LAYER] = { 0,0,0 };
	float2 intersectedMotionVector[N_LAYER][100];

	float4 searchBoundary = motionBoundary(uv, 8.0f);

	for (int i = 0; i < N_RANDOMPICK; i++)
	{
		UV_1 = float2(uv.x + i * 0.0001357f, uv.y + i * 0.0004567f);
		UV_2 = float2(uv.x + i * 0.0002468f, uv.y + i * 0.0003456f);

		randomx = Get_Random(UV_1);
		randomy = Get_Random(UV_2);

		for (int perLayer = 0; perLayer < N_LAYER; perLayer++)
		{
			StartingPoint = float2(searchBoundary.x + randomx * abs(searchBoundary.z - searchBoundary.x),
								   searchBoundary.y + randomy * abs(searchBoundary.w - searchBoundary.y));

			StartingPointMotionVector = g_motionVectorMap.Load(int4(StartingPoint * float2(SCREENWIDTH, SCREENHEIGHT), perLayer, 0)).xy;

			for (int j = 0; j < N_SEARCH; j++)
			{
				if (bIntersecting = IsIntersect(uv, StartingPoint, StartingPointMotionVector))
				{
					break;
				}

				StartingPoint = uv - StartingPointMotionVector * (1.0f / N_RANDOMPICK);
				StartingPointMotionVector = g_motionVectorMap.Load(int4(StartingPoint * float2(SCREENWIDTH, SCREENHEIGHT), perLayer, 0)).xy;
			}

			if (j < N_SEARCH)
			{
				for (int k = 0; k < intersectingCount[perLayer]; k++)
				{
					if (abs(StartingPointMotionVector.x - intersectedMotionVector[perLayer][k].x) < eps &&
						abs(StartingPointMotionVector.y - intersectedMotionVector[perLayer][k].y) < eps)
					{
						break;
					}
				}

				if (k == intersectingCount[perLayer])
				{
					intersectedMotionVector[perLayer][k] = StartingPointMotionVector;
					intersectingCount[perLayer]++;
				}

			}
		}
	}

	float currentPixelDepth = 0.0f;
	float BackwardSampled_Depth = 0.0f;
	float2 BackwardSampled_PixelPosition;
	float3 BackwardSampled_MotionVector;
	int SucessCount = 0;
	float3 sumColor = float3(0.0f, 0.0f, 0.0f);
	float3 sumDepth = float3(0.0f, 0.0f, 0.0f);
	float targetDepth;
	float3 targetColor;

	float2 randomNumber = rand_2_10(uv);


	for (int samplingIndex = 0; samplingIndex < SAMPLINGTIME; samplingIndex++)
	{
		targetColor = float3(-1.0f, 0.0f, 0.0f);
		targetDepth = 1000.0f;

		for (int layer = 0; layer < N_LAYER; layer++)
		{
			for (int k = 0; k < intersectingCount[layer]; k++)
			{
				BackwardSampled_PixelPosition = uv - intersectedMotionVector[layer][k] * ((samplingIndex + randomNumber) / SAMPLINGTIME);
				BackwardSampled_MotionVector = g_motionVectorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), layer, 0)).xyz;
				BackwardSampled_Depth = g_colorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), layer, 0)).w;

				if (distance(intersectedMotionVector[layer][k], BackwardSampled_MotionVector.xy) < 0.0025f)
				{
					BackwardSampled_MotionVector.z *= (float)(samplingIndex + randomNumber) / SAMPLINGTIME;

					if (BackwardSampled_Depth + BackwardSampled_MotionVector.z <= targetDepth)
					{
						targetColor = g_colorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0)).xyz;
						targetDepth = BackwardSampled_Depth + BackwardSampled_MotionVector.z;

						//if (targetColor.x < 0 && targetColor.y < 0)
						//{
						//	targetColor = -targetColor;
						//}

					}
				}
			}
		}

		if (targetColor.r >= 0.0f)
		{
			sumColor += targetColor;
			SucessCount++;
		}
	}

	float3 bluredColor = float3(0, 0, 0);

	if (SucessCount > 0)
	{
		bluredColor = sumColor / SucessCount;
	}

	float SuccessRate = (float)SucessCount / SAMPLINGTIME;

	return float4(bluredColor, SuccessRate);
}


struct FullScreenQuadVertexOut
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

FullScreenQuadVertexOut FullScreenQuadVS(uint vertexID : SV_VertexID)
{
	FullScreenQuadVertexOut OutputVS;
	//---------------------------------------------------------------------------------
	// Option 1: Cull Mode = Back face culling
	//---------------------------------------------------------------------------------

	//<<곱하기
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

//---------------------------------------------------------------------------------
//PASS 4 Denoising
//---------------------------------------------------------------------------------

static const float Gaussiankernel[25] =
{
   0.00390625f,	0.015625f,	0.0234375f,	0.015625f,  0.00390625f,
   0.015625f,	0.0625f,	0.09375f,	0.0625f,	0.015625f,
   0.0234375f,	0.09375f,	0.140625f,	0.09375f,   0.0234375f,
   0.015625f,	0.0625f,	0.09375f,	0.0625f,	0.015625f,
   0.00390625f,	0.015625f,	0.0234375f,	0.015625f,	0.00390625f,
};

static const float2 offsets[25] =
{
   float2(-2.0f, -2.0f), float2(-1.0f, -2.0f), float2(0.0f, -2.0f), float2(1.0f, -2.0f), float2(2.0f, -2.0f),
   float2(-2.0f, -1.0f), float2(-1.0f, -1.0f), float2(0.0f, -1.0f), float2(1.0f, -1.0f), float2(2.0f, -2.0f),
   float2(-2.0f, 0.0f),  float2(-1.0f, 0.0f),  float2(0.0f, 0.0f),  float2(1.0f, 0.0f),  float2(2.0f, 0.0f),
   float2(-2.0f, 1.0f),  float2(-1.0f, 1.0f),  float2(0.0f, 1.0f),  float2(1.0f, 1.0f),  float2(2.0f, 1.0f),
   float2(-2.0f, 2.0f),  float2(-1.0f, 2.0f),  float2(0.0f, 2.0f),  float2(1.0f, 2.0f),  float2(2.0f, 2.0f),
};

float getLuminance(float3 RGB)
{
	return log10((RGB.r * 0.3f + RGB.g * 0.59f + RGB.b * 0.11f) + 1.0f) / log10(2.0f);
}

float4 doEdgeAvoidATrousWavelet_improved(float ScreenWidth, float ScreenHeight, float2 uv, float stepwidth)
{
	const float k = 2.7f; // for mean replacement test k = 2.45f;
	const float epsilonLuminance = 0.000001f;
	const float epsilonDepth = 0.01f;
	float lamda, mipMapLevel, weight, dtmp, depthU, depthV, d_w, d_s, l_w, LuminanceU, LuminanceV, pMin, pMax, cum_w = 0.0f;
	float ltmp, differ, sampledLuminance, SumLuminance = 0.0f, sumLuminanceVariance = 0.0f, miuLuminance, sigmaLuminance2;
	float2 UV, deltaDepth, deltaLuminance, step = float2(1.0f / ScreenWidth, 1.0f / ScreenHeight);
	float3 ntmp, RGBtmp, sum = float3(0.0f, 0.0f, 0.0f);
	float searchSuccessRatetmp = 0.0f;

	//float3 generalColor = g_ColorMap.Sample(g_samPoint, uv).xyz;
	float3 generalColor = g_colorMap.Load(int4(uv * float2(SCREENWIDTH, SCREENHEIGHT), k, 0)).xyz;


	[unroll(25)] for (int j = 0; j < 25; j++)
	{
		UV = uv + offsets[j] * step * stepwidth;

		// Luminance
		RGBtmp = g_motionBlur.Sample(g_samPoint, UV).xyz;
		searchSuccessRatetmp = g_motionBlur.Sample(g_samPoint, UV).w;

		float3 t = generalColor - RGBtmp.xyz;
		float c_w = max(min(1.0 - dot(t, t), 1.0), 0.0);

		l_w = Gaussiankernel[j] * searchSuccessRatetmp;// *c_w;
		//l_w = searchSuccessRatetmp;
		//l_w = Gaussiankernel[j];


		weight = l_w;// *d_s;// *d_s;
		sum += RGBtmp * weight;
		cum_w += weight;
	}

	sum /= cum_w;

	return float4(sum, 1.0f);
}

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

//float4 color = doEdgeAvoidATrousWavelet_improved(SCREENWIDTH, SCREENHEIGHT, In.uv, stepWidth);// 디노이징 한거
//float4 color = g_motionBlur.Sample(g_samLinear, In.uv);// 디노이징 하기전

float4 color = g_colorMap.Load(int4(In.uv * float2(SCREENWIDTH, SCREENHEIGHT), 1, 0));
//float4 color = g_motionVectorMap.Load(int4(In.uv * float2(SCREENWIDTH, SCREENHEIGHT), 1, 0));

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


