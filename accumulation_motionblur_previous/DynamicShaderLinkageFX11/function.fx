//***************************************************************************************
//state and function
//***************************************************************************************
#include "define.h"

void swap(inout uint4 a, inout uint4 b)
{
	uint4 a1 = a;
	a = b;
	b = a1;
}

void BitonicSortF2B(inout uint4 a[MSAA_LEVEL], int n)
{
	int i, j, k;
	[unroll] for (k = 2; k <= n; k = 2 * k) {
		[unroll] for (j = k >> 1; j > 0; j = j >> 1) {
			[unroll] for (i = 0; i < n; i++) {
				int ixj = i ^ j;
				if ((ixj) > i) {
					if ((i & k) == 0 && a[i].y > a[ixj].y) swap(a[i], a[ixj]);
					if ((i & k) != 0 && a[i].y < a[ixj].y) swap(a[i], a[ixj]);
				}
			}
		}
	}
}

float Linearize(float depth)
{
	return (ZFAR * ZNEAR) / (ZFAR - depth * (ZFAR - ZNEAR));
}

float3 homogenious2uv(float4 pos)
{
	float3 position3D = float3(0.0f, 0.0f, 0.0f);

	pos /= pos.w;
	pos.xy += float2(1, 1);
	pos.xy /= float2(2, 2);
	pos.z = Linearize(pos.z);
	position3D = float3(pos.x, pos.y, pos.z);

	return position3D;
}

float Get_Random(float2 uv)
{
	return frac(sin(dot(uv, float2(12.9898f, 78.233f) * 2.0f)) * 43758.5453f);
}


float2 rand_2_10(float2 uv) {
	float noiseX = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
	float noiseY = sqrt(1 - noiseX * noiseX);
	return float2(noiseX, noiseY);
}

float2 GetScreenUV(float2 uv) {
	return uv * float2(SCREENWIDTH, SCREENHEIGHT);
}

bool intersect(float2 target, float3 direction, float2 start, float start_Depth) {

	float threshold = 0.001f;

	float2 startToTarget = target - start;
	float changedTargetPositionX = startToTarget.x * direction.x + startToTarget.y * direction.y;
	float changedTargetPositionY = startToTarget.x * (-direction.y) + startToTarget.y * direction.x;
	float rangeOfX = (direction.x * direction.x) + (direction.y * direction.y);

	float Y = start_Depth + (changedTargetPositionX * direction.z / rangeOfX);

	bool intersecting = false;
	if (changedTargetPositionX <= rangeOfX + threshold && changedTargetPositionX >= -threshold && changedTargetPositionY <= threshold && changedTargetPositionY >= -threshold) {
		intersecting = true;
	}

	return intersecting;
}

//----------------------------------------------------------------


cbuffer cbPerObject
{
	matrix             g_mWorldViewProjection;
	matrix			   g_mPreviousWorldViewProjection;
};
SamplerState g_samLinear;

SamplerState g_samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
	AddressW = Clamp;
};

DepthStencilState NoDepthNoStencil_DS
{
	DepthEnable = false;
	StencilEnable = false;
};

RasterizerState BackCullNoMSAA_RS
{
	CullMode = Back;
	FillMode = Solid;
	MultisampleEnable = false;
};

RasterizerState NoCullNoMSAA_RS
{
	CullMode = None;
	FillMode = Solid;
	MultisampleEnable = false;
};

BlendState NoMRT_BS
{
	RenderTargetWriteMask[0] = 0x0f;
};

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
