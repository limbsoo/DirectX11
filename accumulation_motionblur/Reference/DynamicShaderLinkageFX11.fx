//-----------------------------------------------------------------------------
// File: shader.fx
// The HLSL file for the DDSWithoutD3DX sample for the Direct3D 11 device
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

//#include "LightHelper.fx"
#include "define.h"

cbuffer cbPerObject : register(b0)
{
	matrix             g_worldViewProjection;
	matrix			   g_previousWorldViewProjection; //previous only this  ,other all current//g_mPreviousWorldViewProjection
	matrix			   g_MVPInverseTransposed;
	matrix             g_worldView;
	matrix             g_world;
};


SamplerState    g_samLinear
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;

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


//-----------------------------------------------------------------------------
// 	   Render reference per layer
//-----------------------------------------------------------------------------


Texture2D<float4>		g_txDiffuse;

struct VS_INPUT
{
	float4 Position    : POSITION; // vertex position 	
	float2 TextureUV   : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position     : SV_POSITION;
	float2 TexCoord     : TEXCOORD0;
};

VS_OUTPUT ReferenceRendering_VS(VS_INPUT InputVS)
{
	VS_OUTPUT OutputVS = (VS_OUTPUT)0;

	OutputVS.Position = mul(InputVS.Position, g_previousWorldViewProjection);
	OutputVS.TexCoord = InputVS.TextureUV;

	return OutputVS;
}

float4 ReferenceRendering_PS(VS_OUTPUT InputPS) : SV_TARGET
{
	float4 color = g_txDiffuse.Sample(g_samLinear, InputPS.TexCoord , 0);
	return color;
}

technique11 ReferenceRendering
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, ReferenceRendering_VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ReferenceRendering_PS()));

		SetRasterizerState(BackCullNoMSAA_RS);
		SetDepthStencilState(DepthNoStencil_DS, 0x00000000);
		SetBlendState(NoMRT_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
	}
}




//-----------------------------------------------------------------------------
// 	   MakeMotionBlur
//-----------------------------------------------------------------------------


Texture2DArray<float4>	g_Ref_Primary_colorMap;
Texture2DArray<float4>	g_Ref_Secondary_colorMap;
Texture2DArray<float4>	g_Ref_Tertiary_colorMap;
Texture2DArray<float4>	g_Ref_Quaternary_colorMap;

Texture2DArray<float4>	g_Ref_5_colorMap;
Texture2DArray<float4>	g_Ref_6_colorMap;
Texture2DArray<float4>	g_Ref_7_colorMap;
Texture2DArray<float4>	g_Ref_8_colorMap;

struct QuadOutput
{
	float4 pos : SV_Position;
	float2 tex : TEXCOORD0;
};

QuadOutput FullScreenTriVS(uint id : SV_VertexID) //≤¿¡ˆ¡° INDEX 0,1,2
{
	QuadOutput output = (QuadOutput)0.0f;
	output.tex = float2((id << 1) & 2, id & 2);
	output.pos = float4(output.tex * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	return output;
}

float4 Ref_MotionBlurPS(QuadOutput IN) : SV_TARGET
{
	float4 color = float4(0.0f, 0.0f, 0.0f,0.0f);

	for (int i = 0; i < 4; i++) 
	{
		color += float4(g_Ref_Primary_colorMap.Load(int4(IN.tex * float2(SCREENWIDTH, SCREENHEIGHT), i, 0)).xyz, 0);
	}

	if (MAXSUBFRAME > 8)
	{
		for (int i = 0; i < 8; i++)
		{
			color += float4(g_Ref_Secondary_colorMap.Load(int4(IN.tex * float2(SCREENWIDTH, SCREENHEIGHT), i, 0)).xyz, 0);
		}
	}
	
	if (MAXSUBFRAME > 16)
	{
		for (int i = 0; i < 8; i++)
		{
			color += float4(g_Ref_Tertiary_colorMap.Load(int4(IN.tex * float2(SCREENWIDTH, SCREENHEIGHT), i, 0)).xyz, 0);
		}
	}

	if (MAXSUBFRAME > 24)
	{
		for (int i = 0; i < 8; i++)
		{
			color += float4(g_Ref_Quaternary_colorMap.Load(int4(IN.tex * float2(SCREENWIDTH, SCREENHEIGHT), i, 0)).xyz, 0);
		}
	}



	if (MAXSUBFRAME > 32)
	{
		for (int i = 0; i < 8; i++)
		{
			color += float4(g_Ref_5_colorMap.Load(int4(IN.tex * float2(SCREENWIDTH, SCREENHEIGHT), i, 0)).xyz, 0);
		}
	}

	if (MAXSUBFRAME > 40)
	{
		for (int i = 0; i < 8; i++)
		{
			color += float4(g_Ref_6_colorMap.Load(int4(IN.tex * float2(SCREENWIDTH, SCREENHEIGHT), i, 0)).xyz, 0);
		}
	}

	if (MAXSUBFRAME > 48)
	{
		for (int i = 0; i < 8; i++)
		{
			color += float4(g_Ref_7_colorMap.Load(int4(IN.tex * float2(SCREENWIDTH, SCREENHEIGHT), i, 0)).xyz, 0);
		}
	}

	if (MAXSUBFRAME > 54)
	{
		for (int i = 0; i < 8; i++)
		{
			color += float4(g_Ref_8_colorMap.Load(int4(IN.tex * float2(SCREENWIDTH, SCREENHEIGHT), i, 0)).xyz, 0);
		}
	}


	color = color / MAXSUBFRAME;

	return color;
}

technique11 Ref_MotionBlur
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, FullScreenTriVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, Ref_MotionBlurPS()));
		SetRasterizerState(rasterizerState_New);

	}
}

