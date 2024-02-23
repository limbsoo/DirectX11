//-----------------------------------------------------------------------------
// File: shader.fx
// The HLSL file for the DDSWithoutD3DX sample for the Direct3D 11 device
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "function.fx" 

Texture2D g_modelTexture;

Texture2DArray< float4 > g_1at8Frame;
Texture2DArray< float4 > g_9at16Frame;
Texture2DArray< float4 > g_17at24Frame;
Texture2DArray< float4 > g_25at32Frame;

//------------------------------------------------------------- Rendering pass

struct RenderingVSInput
{
	float4 Position    : POSITION;
	float3 Normal	   : NORMAL;
	float2 uv   : TEXCOORD0;
};

struct RenderingVSOut
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD0;
};

RenderingVSOut RenderingVS(RenderingVSInput Input)
{
	RenderingVSOut OutputVS;

	OutputVS.position = mul(Input.Position, g_mWorldViewProjection);

	//OutputVS.position /= OutputVS.position.w;

	//OutputVS.position.xyz = homogenious2uv(OutputVS.position);
	
	OutputVS.uv = Input.uv;

	return OutputVS;
}

float4 RenderingPS(RenderingVSOut input) : SV_TARGET
{
	return g_modelTexture.Sample(g_samPoint, input.uv);
}

technique11 Rendering
{
	pass P0
	{
		SetRasterizerState(BackCullNoMSAA_RS);
		SetVertexShader(CompileShader(vs_5_0, RenderingVS()));
		SetPixelShader(CompileShader(ps_5_0, RenderingPS()));
		SetBlendState(NoMRT_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
	}
}

//------------------------------------------------------------- denoising pass

struct FSQVSInput
{
	float4 Position    : POSITION;
	float3 Normal      : NORMAL;
	float2 uv   : TEXCOORD0;
};

struct FSQVSOut
{
	float4 position : SV_POSITION;
	float2 uv   : TEXCOORD0;
};

FSQVSOut FSQVS(FSQVSInput input)
{
	FSQVSOut OutputVS;

	OutputVS.position = input.Position;
	OutputVS.uv = input.uv;
	return OutputVS;
}

float4 motionBlurPS(FSQVSOut input) : SV_TARGET
{

	float4 color = float4(0,0,0,0);

	for (int i = 0; i < TEXTUREARRSIZE; i++) {
		color += g_1at8Frame.Load(int4(GetScreenUV(input.uv), i, 0));
	}
	
	if (MAXSUBFRAME > 8) {
		for (int j = 0; j < TEXTUREARRSIZE; j++) {
			color += g_9at16Frame.Load(int4(GetScreenUV(input.uv), j, 0));
		}
	}
	if (MAXSUBFRAME > 16) {
		for (int k = 0; k < TEXTUREARRSIZE; k++) {
			color += g_17at24Frame.Load(int4(GetScreenUV(input.uv), k, 0));
		}
	}
	if (MAXSUBFRAME > 24) {
		for (int a = 0; a < TEXTUREARRSIZE; a++) {
			color += g_25at32Frame.Load(int4(GetScreenUV(input.uv), a, 0));
		}
	}

	color = color / MAXSUBFRAME;
	if (MAXSUBFRAME == 1) {
		color = g_1at8Frame.Load(int4(GetScreenUV(input.uv), 0, 0));
	}

	return color;
}

technique11 motionBlur
{
	pass P0
	{
		SetRasterizerState(NoCullNoMSAA_RS);
		SetVertexShader(CompileShader(vs_5_0, FSQVS()));
		SetPixelShader(CompileShader(ps_5_0, motionBlurPS()));
	}
}