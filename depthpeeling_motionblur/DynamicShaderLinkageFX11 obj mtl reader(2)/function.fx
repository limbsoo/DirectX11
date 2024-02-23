//***************************************************************************************
//state and function
//***************************************************************************************
#include "define.h"

float2 g_pixelSize = float2(1.f / SCREENWIDTH, 1.f / SCREENHEIGHT);

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
	pos.xy /= float2(2, -2);
	pos.z = Linearize(pos.z);
	position3D = float3(pos.x, pos.y, pos.z);

	return position3D;
}

float Random(float2 uv)
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

bool intersect(float2 target, float2 vec, float2 start, float epsilon) {

	bool intersecting = false;

	float2 startToTarget = target - start;
	float segmentLengthSqr = vec.x * vec.x + vec.y * vec.y;
	float r = (startToTarget.x * vec.x + startToTarget.y * vec.y) / segmentLengthSqr;

	if (r > 0 && r < 1) {
		float2 targetToStart = start - target;
		float sl = (targetToStart.y * vec.x - targetToStart.x * vec.y) / sqrt(segmentLengthSqr);

		if (-epsilon <= sl && sl <= epsilon) intersecting = true;
	}

	return intersecting;
}

//----------------------------------------------------------------

uint pack_rgb(float3 c)
{
	uint3 c3 = (uint3) (c * 0xff);
	return c3.r + (c3.g << 8) + (c3.b << 16);
}

float3 unpack_rgb(uint c)
{
	float3 c3 = float3(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff);
	return c3 / 0xff;
}

uint float2Uint16(float x)
{
	float temp = (x - MOTIONVECTOR_MIN) / (MOTIONVECTOR_MAX - MOTIONVECTOR_MIN);
	return (uint)(temp * 0xffff);
}

uint pack_MotionVector_XY(float2 v)
{
	return float2Uint16(v.x) + (float2Uint16(v.y) << 16);
}

float2 unpack_MotionVector_XY(uint v)
{
	float2 v2;

	v2.x = v & 0xffff;
	v2.y = (v >> 16) & 0xffff;

	float2 res;
	res.x = (v2.x / 0xffff) * (MOTIONVECTOR_MAX - MOTIONVECTOR_MIN) + MOTIONVECTOR_MIN;
	res.y = (v2.y / 0xffff) * (MOTIONVECTOR_MAX - MOTIONVECTOR_MIN) + MOTIONVECTOR_MIN;

	return res;
}

uint pack_MotionVector_Z(float z)
{
	float dpeth = (z - MOTIONVECTOR_MIN) / (MOTIONVECTOR_MAX - MOTIONVECTOR_MIN);
	uint Uintdpeth = dpeth * 0xffffffff;
	return Uintdpeth;
}

float unpack_MotionVector_Z(uint z)
{
	float depth;

	if (z == 0) depth = MOTIONVECTOR_MIN;
	else {
		depth = (float)z / 0xffffffff;
		depth = depth * (MOTIONVECTOR_MAX - MOTIONVECTOR_MIN) + MOTIONVECTOR_MIN;
	}
	return depth;

}

uint pack_depth(float z)
{
	return (uint) ((z - ZNEAR) / (ZFAR - ZNEAR) * 0xffffffff);
}

float unpack_depth(uint z)
{
	return ((float)z / 0xffffffff) * (ZFAR - ZNEAR) + ZNEAR;
}

//----------------------------------------------------------------

cbuffer cbPerObject
{
	matrix             g_worldViewProjection;
	matrix			   g_previousWorldViewProjection;
	matrix             g_world;
};
SamplerState g_samLinear;

SamplerState g_samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
};

DepthStencilState NoDepthNoStencil_DS
{
	DepthEnable = false;
	StencilEnable = false;
};

DepthStencilState DepthNoStencil_DS
{
	DepthEnable = true;
	DepthFunc = Less_Equal;
	DepthWriteMask = All;
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

RasterizerState rasterizerState_New
{
	FillMode = SOLID;
	CullMode = NONE;
};


BlendState NoMRT_BS
{
	RenderTargetWriteMask[0] = 0x0f;
};

BlendState MRT2_BS
{
	RenderTargetWriteMask[0] = 0x0f;
	RenderTargetWriteMask[1] = 0x0f;
};

static const float gaussianKernel[25] =
{
	0.00390625f, 0.015625f,   0.0234375f,   0.015625f,   0.00390625f,
	0.015625f,    0.0625f,   0.09375f,   0.0625f,   0.015625f,
	0.0234375f,    0.09375f,   0.140625f,   0.09375f,   0.0234375f,
	0.015625f,    0.0625f,   0.09375f,   0.0625f,   0.015625f,
	0.00390625f, 0.015625f,   0.0234375f,   0.015625f,   0.00390625f,
};

static const float2 offsets[25] =
{
	float2(-2.0f, -2.0f) * g_pixelSize, float2(-1.0f, -2.0f) * g_pixelSize, float2(0.0f, -2.0f) * g_pixelSize, float2(1.0f, -2.0f) * g_pixelSize, float2(2.0f, -2.0f) * g_pixelSize,
	float2(-2.0f, -1.0f) * g_pixelSize, float2(-1.0f, -1.0f) * g_pixelSize, float2(0.0f, -1.0f) * g_pixelSize, float2(1.0f, -1.0f) * g_pixelSize, float2(2.0f, -1.0f) * g_pixelSize,
	float2(-2.0f, 0.0f) * g_pixelSize, float2(-1.0f, 0.0f) * g_pixelSize,  float2(0.0f, 0.0f) * g_pixelSize,  float2(1.0f, 0.0f) * g_pixelSize,  float2(2.0f, 0.0f) * g_pixelSize,
	float2(-2.0f, 1.0f) * g_pixelSize, float2(-1.0f, 1.0f) * g_pixelSize,  float2(0.0f, 1.0f) * g_pixelSize,  float2(1.0f, 1.0f) * g_pixelSize,  float2(2.0f, 1.0f) * g_pixelSize,
	float2(-2.0f, 2.0f) * g_pixelSize, float2(-1.0f, 2.0f) * g_pixelSize,  float2(0.0f, 2.0f) * g_pixelSize,  float2(1.0f, 2.0f) * g_pixelSize,  float2(2.0f, 2.0f) * g_pixelSize,
};
