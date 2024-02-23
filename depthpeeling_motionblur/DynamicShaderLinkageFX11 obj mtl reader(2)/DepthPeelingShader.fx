#include "define.h"

cbuffer cbPerObject
{
    matrix             g_worldViewProjection;
    matrix			   g_previousWorldViewProjection;
    matrix             g_world;
};

// Textures and Samplers
Texture2D            g_modelTexture;

Texture2DArray<float4>   g_colorMap;
Texture2DArray<float4>   g_motionVectorMap;
Texture2DArray<float4>   g_motionVectorSquareMap;
Texture2D<float4>      g_depthBuffer;

Texture2D            g_motionBlur;
Texture2D            g_denosing;

Texture2DArray<float4>   g_normalMap;

SamplerState    g_samPoint
{
   Filter = MIN_MAG_MIP_POINT;
   AddressU = WRAP;
   AddressV = WRAP;
   AddressW = WRAP;

};

//SamplerState    g_samLinear1;

SamplerState   g_samLinear
{
    //AddressU = WRAP;
    //AddressV = WRAP;
    //AddressW = WRAP;
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

float Linearize(float depth, float near, float far)
{
   return (far * near) / (far - depth * (far - near));
} 

float3 homogeneous2uv(float4 pos)
{
   float3 position3D = float3(0.0f, 0.0f, 0.0f);

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

//-----------------------------------------------------------------------------
//       1PASS:   Clear Texture
//-----------------------------------------------------------------------------

struct QuadOutput
{
   float4 pos : SV_Position;
   float2 tex : TEXCOORD0;
};

QuadOutput FullScreenTriVS(uint id : SV_VertexID)
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


DepthColorOutputMRT ClearPS(QuadOutput IN)
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
      SetVertexShader(CompileShader(vs_5_0, FSQVS()));

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
//       2PASS:   DepthPeeling Render
//-----------------------------------------------------------------------------

//float4 DepthPeelingPostPS(QuadOutput IN) : SV_TARGET
//{
//   float4 color = float4(0.0f, 0.0f, 0.0f,0.0f);
//
//   color = float4(g_colorMap.Load(int4(IN.tex * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0)).xyz, 0);
//
//   return color;
//}
//
//
//technique11 LayersDrawOnFullscreenquad
//{
//   pass P0
//   {
//      SetVertexShader(CompileShader(vs_5_0, FullScreenTriVS()));
//      SetGeometryShader(NULL);
//      SetPixelShader(CompileShader(ps_5_0, DepthPeelingPostPS()));
//
//      SetRasterizerState(rasterizerState_New);
//   }
//}
//

struct VS_INPUT
{
   float4 Position    : POSITION; 
   float3 Normal      : NORMAL;
   float2 TextureUV   : TEXCOORD0;
};

struct VS_OUTPUT
{
   float4 Position     : SV_POSITION;
   float4 prePosition  : POSITION0;
   float4 curPosition  : POSITION1;
   float3 Normal       : NORMAL;
   float2 TexCoord     : TEXCOORD0;  

   float4 WorldPos : POSITION2;
};

struct RenderOutput_PS
{
   float4 color;
   float4 motionVector3D;
   float4 motionVector3DSquare;
   float3 outNormal;
};

VS_OUTPUT DepthPeelingRendering_VS(VS_INPUT InputVS)
{
   VS_OUTPUT OutputVS = (VS_OUTPUT)0;

   OutputVS.Position = mul(InputVS.Position, g_previousWorldViewProjection);
   OutputVS.prePosition = OutputVS.Position;
   OutputVS.curPosition = mul(InputVS.Position, g_worldViewProjection);
   OutputVS.TexCoord = InputVS.TextureUV;

   //OutputVS.Normal = InputVS.Normal;
   //float3 n = normalize(mul(float4(InputVS.Normal, 0.f), g_world)).xyz;
   //float3 n = mul(float4(InputVS.Normal, 0.f), g_world).xyz;
   //OutputVS.Normal = normalize(mul(float4(InputVS.Normal, 0.f), g_world));
   //n = normalize(n);
   //OutputVS.Normal = float3(n.x, n.y, n.z);
   //OutputVS.Normal = n.xyz;

   OutputVS.WorldPos = mul(InputVS.Position, g_world);
   OutputVS.Normal = mul(float4(InputVS.Normal, 0.f), g_world).xyz;
   OutputVS.Normal = normalize(OutputVS.Normal);
   //OutputVS.Normal = mul(InputVS.Normal,g_world);
   //OutputVS.Normal = mul(InputVS.Normal, (float3x3)g_world);



   return OutputVS;
}

RenderOutput_PS DepthPeelingRendering_PS(VS_OUTPUT InputPS) : SV_TARGET
{
   float3 currentPos = homogeneous2uv(InputPS.curPosition);
   float3 previousPos = homogeneous2uv(InputPS.prePosition);
   float3 motionVector = currentPos - previousPos;
   float3 motionVectorSquare = float3(motionVector.x * motionVector.x, motionVector.y * motionVector.y, motionVector.z * motionVector.z);
   float4 color = g_modelTexture.Sample(g_samPoint, InputPS.TexCoord , 0);
   float depth = Linearize(InputPS.Position.z, 0.1f, 1000.0f);

   RenderOutput_PS outputPS;
   outputPS.color = float4(color.xyz, depth);
   outputPS.motionVector3D = float4(motionVector, 1.0f);
   outputPS.motionVector3DSquare = float4(motionVectorSquare, 1.0f);

   outputPS.outNormal = InputPS.Normal;


   float3 ambientLightColor = float3(0.1f, 0.1f, 0.1f);
   float ambientLightStrength = 0.01f;
   float3 dynamicLightPosition = float3(15.f, -6.f, 16.f);
   float dynamicLightStrength = 0.9f;
   float3 dynamicLightColor = float3(1.f, 1.f, 1.f);

   float3 ambientLight = ambientLightColor * ambientLightStrength;
   float3 appliedLight = ambientLight;
   float3 vectorToLight = normalize(dynamicLightPosition - InputPS.WorldPos.xyz).xyz;
   float3 diffuseLightIntensity = max(dot(vectorToLight, outputPS.outNormal), 0);
   float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;
   appliedLight += diffuseLight;
   float3 finalColor = color.xyz * appliedLight;
   outputPS.color = float4(finalColor, depth);


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


technique11 DepthPeelingRenderingWithSkybox
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, DepthPeelingRendering_VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, DepthPeelingRendering_PS()));

        //SetRasterizerState(rasterizerState_New);

        SetRasterizerState(NoCullNoMSAA_RS);
        SetDepthStencilState(DepthNoStencil_DS, 0x00000000);
        SetBlendState(NoMRT_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
    }
}


//---------------------------------------------------------------------------------
//PASS 3 Make MotionBlur on FullSceenQuad
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


float4 MotionBoundary(float2 uv, float mipmapLevelForGaussian)
{
   const int constant =3;

   float2 mean = g_motionVectorMap.SampleLevel(g_samLinear, float3(uv, 0), mipmapLevelForGaussian).xy;
   float2 squaMean = g_motionVectorSquareMap.SampleLevel(g_samLinear, float3(uv, 0), mipmapLevelForGaussian).xy;

   //float2 mean = g_motionVectorMap.SampleLevel(g_samPoint, float3(uv, 0), mipmapLevelForGaussian).xy;
   //float2 squaMean = g_motionVectorSquareMap.SampleLevel(g_samPoint, float3(uv, 0), mipmapLevelForGaussian).xy;

   float2 standardDeviation = sqrt(squaMean - mean * mean);//Á¦°öÀÇ Æò±Õ -Æò±ÕÀÇ Á¦°ö : Ç¥ÁØÆíÂ÷ Á¦°ö =>ºÐ»ê
   float2 Max = mean + standardDeviation * constant;
   float2 Min = mean - standardDeviation * constant;

   float2 leftTopCorner = max(uv - Max, float2(0.0f, 0.0f));
   float2 rightBottomCorner = min(uv - Min, float2(1.f, 1.0f));

   return float4 (leftTopCorner, rightBottomCorner);
}


bool IsIntersect(float2 uv, float2 lineStart, float2 lineMotionVector)
{

   bool Intersected = false;

   static const float eps = 0.5f;

   static const float threshold_X = 0.5f / SCREENWIDTH;
   static const float threshold_Y = 0.5f / SCREENHEIGHT;

   if (distance(uv.x, lineStart.x) >= threshold_X && distance(uv.x, lineStart.x) <= -threshold_X &&
      distance(uv.y, lineStart.y) >= threshold_Y && distance(uv.y, lineStart.y) <= -threshold_Y)
   {
      Intersected = true;
   }

   else if (lineMotionVector.x <= eps && lineMotionVector.x >= -eps && lineMotionVector.y <= eps && lineMotionVector.y >= -eps)
   {
      Intersected = true;
   }

   else
   {

      static const float2 pixelSize = float2(1 / SCREENWIDTH, 1 / SCREENHEIGHT);
      static const float epsilon = pixelSize.y * 7.f;

      float2 RS_Start = lineStart;
      float2 RS_Vector = lineMotionVector;
      float2 RS_Point = uv;

      float segmentLengthSqr = (RS_Vector.x * RS_Vector.x) + (RS_Vector.y * RS_Vector.y);
      float r = ((RS_Point.x - RS_Start.x) * RS_Vector.x + (RS_Point.y - RS_Start.y) * RS_Vector.y) / segmentLengthSqr;

      if (r > 0 && r < 1)
      {
         float sl = ((RS_Start.y - RS_Point.y) * RS_Vector.x - (RS_Start.x - RS_Point.x) * (RS_Vector.y)) / sqrt(segmentLengthSqr);

         if (-epsilon <= sl && sl <= epsilon)
         {
            Intersected = true;
         }

      }
   }


   return Intersected;


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

struct FullScreenQuadVertexOut 
{
   float4 position : SV_POSITION;
   float2 uv : TEXCOORD0;
};

FullScreenQuadVertexOut FullScreenQuadVS(uint vertexID : SV_VertexID)
{
   FullScreenQuadVertexOut OutputVS;

   OutputVS.uv = float2((vertexID << 1) & 2, vertexID & 2);
   OutputVS.position = float4(OutputVS.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

   return OutputVS;
}



//float4 FullScreenQuadPS(FullScreenQuadVertexOut In) : SV_TARGET
//{
//    float4 finalColor = float4(0, 0, 0, 0);
//    finalColor = g_colorMap.Load(int4(In.uv * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0)).xyzw;
//    //finalColor = g_normalMap.Load(int4(In.uv * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0)).xyzw;
//    
//
//    finalColor.w = 0.f;
//
//     return finalColor;
//}


float4 FullScreenQuadPS(FullScreenQuadVertexOut In) : SV_TARGET
{
   //static const float2 g_pixelSize = float2(1.f / SCREENWIDTH, 1.f / SCREENHEIGHT);
   static const float2 pixelSize = float2(1 / SCREENWIDTH, 1 / SCREENHEIGHT);
   static const float epsilon = pixelSize.y * EPSILON;

   float2 intersectedMotionVector[N_LAYER * N_RANDOMPICK];
   int intersectingCount = 1;
   intersectedMotionVector[0] = g_motionVectorMap.Load(int4(In.uv * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0)).xy;

   float2 randomUV;
   float2 StartingPoint = float2(0.0f, 0.0f);
   float2 StartingPointMotionVector;

   float4 searchBoundary = MotionBoundary(In.uv, MIPMAP_LEVEL);

   for (int i = 0; i < N_RANDOMPICK; i++)
   {

      for (int perLayer = 0; perLayer < N_LAYER; perLayer++)
      {
         bool isFind = false;

         randomUV = float2(Get_Random(float2(In.uv.x + i * 0.0002468f, In.uv.y + i * 0.0003456f)),
                    Get_Random(float2(In.uv.x + i * 0.0001357f, In.uv.y + i * 0.0004567f)));

         StartingPoint = float2(searchBoundary.x + randomUV.x * abs(searchBoundary.z - searchBoundary.x),
                         searchBoundary.y + randomUV.y * abs(searchBoundary.w - searchBoundary.y));

         StartingPointMotionVector = g_motionVectorMap.Load(int4(StartingPoint * float2(SCREENWIDTH, SCREENHEIGHT), perLayer, 0)).xy;

         for (float j = 0; j < N_SEARCH; j++)
         {
            //float2 randomNumber = rand_2_10(In.uv);

            //if (intersect(In.uv, StartingPointMotionVector, StartingPoint, epsilon))
            if (IsIntersect(In.uv, StartingPoint, StartingPointMotionVector))
            {
               isFind = true;
               break;
            }

            else
            {
               //StartingPoint = In.uv - (StartingPointMotionVector * (j + randomNumber / (N_RANDOMPICK)));
               StartingPoint = In.uv - StartingPointMotionVector * j / N_RANDOMPICK;
               StartingPointMotionVector = g_motionVectorMap.Load(int4(StartingPoint * float2(SCREENWIDTH, SCREENHEIGHT), perLayer, 0)).xy;
            }
         }

         if (isFind)
         {
            for (int k = 0; k < intersectingCount; k++)
            {

               //if (distance(StartingPointMotionVector, intersectedMotionVector[k]) < pixelSize.y) break;
               if (abs(StartingPointMotionVector.x - intersectedMotionVector[k].x) < pixelSize.x &&
                  abs(StartingPointMotionVector.y - intersectedMotionVector[k].y) < pixelSize.y)
               {
                  break;
               }
            }

            if (k == intersectingCount)
            {
               intersectedMotionVector[k] = StartingPointMotionVector;
               intersectingCount++;
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

   float4 finalColor = float4(0, 0, 0, 0);

   //if (intersectingCount == 1 && -pixelSize.x < intersectedMotionVector[0].x && intersectedMotionVector[0].x < pixelSize.x && -pixelSize.y < intersectedMotionVector[0].y && intersectedMotionVector[0].y < pixelSize.y) {
   //    float4 NonMotionBlurColor = g_colorMap.Load(int4(In.uv * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0));
   //    //output.dpeth_successRate_motionVector = float4(unpack_depth(firstLayer.y), 1, 0, 0);
   //    finalColor = float4(NonMotionBlurColor.xyz, 0);
   //}

   //else
   {
       for (float samplingTime_idx = 0; samplingTime_idx < N_SAMPLINGTIME; samplingTime_idx++)
       {
           float3 targetColor = float3(-1.0f, 0.0f, 0.0f);
           float targetDepth = 1000.f;

           randomUV = float2(Get_Random(float2(In.uv.x + samplingTime_idx * 0.0002468f, In.uv.y + samplingTime_idx * 0.0003456f)),
                             Get_Random(float2(In.uv.x + samplingTime_idx * 0.0001357f, In.uv.y + samplingTime_idx * 0.0004567f)));

           //int perLayer = 0;
           for (int perLayer = 0; perLayer < N_LAYER; perLayer++)
           {
               //¸ð¼Çº¤ÅÍ°¹¼ö
               for (int k = 0; k < intersectingCount; k++)
               {
                   BackwardSampled_PixelPosition = In.uv - intersectedMotionVector[k] * (samplingTime_idx + randomUV) / N_SAMPLINGTIME;
                   //BackwardSampled_PixelPosition = In.uv - (intersectedMotionVector[k] * (8.f / N_SAMPLINGTIME));
                   BackwardSampled_MotionVector = g_motionVectorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), perLayer, 0)).xyz;
                   BackwardSampled_Depth = g_colorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), perLayer, 0)).w;

                   if (distance(intersectedMotionVector[k], BackwardSampled_MotionVector.xy) < 0.005f)
                   {
                       BackwardSampled_MotionVector.z *= (float)(samplingTime_idx + randomUV) / N_SAMPLINGTIME;
                       //BackwardSampled_MotionVector.z *= float(12.f / SAMPLINGTIME);

                       if (BackwardSampled_Depth + BackwardSampled_MotionVector.z < targetDepth)
                       {
                           targetColor = g_colorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), perLayer, 0)).xyz;
                           //targetColor = g_motionVectorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), perLayer, 0)).xyz;

                           targetDepth = BackwardSampled_Depth + BackwardSampled_MotionVector.z;

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

       float SuccessRate = (float)SucessCount / N_SAMPLINGTIME;
       finalColor = float4(bluredColor, SuccessRate);
   }



   return finalColor;
}



//float4 FullScreenQuadPS(FullScreenQuadVertexOut In) : SV_TARGET
//{
//   //static const float2 g_pixelSize = float2(1.f / SCREENWIDTH, 1.f / SCREENHEIGHT);
//   static const float2 pixelSize = float2(1 / SCREENWIDTH, 1 / SCREENHEIGHT);
//   static const float epsilon = pixelSize.y * EPSILON;
//
//   float2 intersectedMotionVector[N_LAYER * N_RANDOMPICK];
//   int intersectingCount = 1;
//   intersectedMotionVector[0] = g_motionVectorMap.Load(int4(In.uv * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0)).xy;
//
//   float2 randomUV;
//   float2 StartingPoint = float2(0.0f, 0.0f);
//   float2 StartingPointMotionVector;
//
//   float4 searchBoundary = MotionBoundary(In.uv, MIPMAP_LEVEL);
//
//
//   float4 finalColor = float4(0, 0, 0, 0);
//
//   for (int i = 0; i < N_RANDOMPICK; i++)
//   {
//
//      for (int perLayer = 0; perLayer < N_LAYER; perLayer++)
//      {
//         bool isFind = false;
//
//         randomUV = float2(Get_Random(float2(In.uv.x + i * 0.0002468f, In.uv.y + i * 0.0003456f)),
//                    Get_Random(float2(In.uv.x + i * 0.0001357f, In.uv.y + i * 0.0004567f)));
//
//         StartingPoint = float2(searchBoundary.x + randomUV.x * abs(searchBoundary.z - searchBoundary.x),
//                         searchBoundary.y + randomUV.y * abs(searchBoundary.w - searchBoundary.y));
//
//         StartingPointMotionVector = g_motionVectorMap.Load(int4(StartingPoint * float2(SCREENWIDTH, SCREENHEIGHT), perLayer, 0)).xy;
//
//         for (float j = 0; j < N_SEARCH; j++)
//         {
//            //float2 randomNumber = rand_2_10(In.uv);
//
//            //if (intersect(In.uv, StartingPointMotionVector, StartingPoint, epsilon))
//            if (IsIntersect(In.uv, StartingPoint, StartingPointMotionVector))
//            {
//               isFind = true;
//
//               if (StartingPointMotionVector.x != 0 && StartingPointMotionVector.y != 0)
//               {
//                   finalColor += float4(0.1, 0, 0, 0);
//               }
//
//               break;
//            }
//
//            else
//            {
//               //StartingPoint = In.uv - (StartingPointMotionVector * (j + randomNumber / (N_RANDOMPICK)));
//               StartingPoint = In.uv - StartingPointMotionVector * j / N_RANDOMPICK;
//               StartingPointMotionVector = g_motionVectorMap.Load(int4(StartingPoint * float2(SCREENWIDTH, SCREENHEIGHT), perLayer, 0)).xy;
//            }
//         }
//
//         if (isFind)
//         {
//            for (int k = 0; k < intersectingCount; k++)
//            {
//
//               //if (distance(StartingPointMotionVector, intersectedMotionVector[k]) < pixelSize.y) break;
//               if (abs(StartingPointMotionVector.x - intersectedMotionVector[k].x) < pixelSize.x &&
//                  abs(StartingPointMotionVector.y - intersectedMotionVector[k].y) < pixelSize.y)
//               {
//                  break;
//               }
//            }
//
//            if (k == intersectingCount)
//            {
//               intersectedMotionVector[k] = StartingPointMotionVector;
//               intersectingCount++;
//            }
//
//         }
//
//      }
//   }
//
//
//
//   //float currentPixelDepth = 0.0f;
//   //float BackwardSampled_Depth = 0.0f;
//   //float2 BackwardSampled_PixelPosition;
//   //float3 BackwardSampled_MotionVector;
//   //int SucessCount = 0;
//   //float3 sumColor = float3(0.0f, 0.0f, 0.0f);
//   //float3 sumDepth = float3(0.0f, 0.0f, 0.0f);
//
//   // for (int k = 0; k < intersectingCount; k++)
//   // {
//   //     BackwardSampled_PixelPosition = In.uv - intersectedMotionVector[k] /** (samplingTime_idx + randomUV)*/;
//   //     //BackwardSampled_PixelPosition = In.uv - (intersectedMotionVector[k] * (8.f / N_SAMPLINGTIME));
//   //     BackwardSampled_MotionVector = g_motionVectorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0)).xyz;
//   //     BackwardSampled_Depth = g_colorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0)).w;
//
//   //     if (distance(intersectedMotionVector[k], BackwardSampled_MotionVector.xy) < 0.005f)
//   //     {
//   //         //BackwardSampled_MotionVector.z *= (float)(samplingTime_idx + randomUV) / N_SAMPLINGTIME;
//   //         ////BackwardSampled_MotionVector.z *= float(12.f / SAMPLINGTIME);
//
//   //         //if (BackwardSampled_Depth + BackwardSampled_MotionVector.z < targetDepth)
//   //         //{
//   //         //    targetColor = g_colorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), perLayer, 0)).xyz;
//   //         //    //targetColor = g_motionVectorMap.Load(int4(BackwardSampled_PixelPosition * float2(SCREENWIDTH, SCREENHEIGHT), perLayer, 0)).xyz;
//
//   //         //    targetDepth = BackwardSampled_Depth + BackwardSampled_MotionVector.z;
//
//   //         //}
//
//   //         finalColor += float4(0.1, 0, 0, 0);
//   //     }
//
//   // }
//   // 
//
//   ////finalColor = g_colorMap.Load(int4(In.uv * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0)).xyzw;
//
//   return finalColor;
//}
//















technique11 MotionBlur
{
   pass P0
   {
      SetVertexShader(CompileShader(vs_5_0, FSQVS()));

      SetPixelShader(CompileShader(ps_5_0, FullScreenQuadPS()));
      SetRasterizerState(BackCullNoMSAA_RS);
   }

}

//---------------------------------------------------------------------------------
//PASS 4 Denoising
//---------------------------------------------------------------------------------

static const float Gaussiankernel[25] =
{
   0.00390625f,   0.015625f,   0.0234375f,   0.015625f,  0.00390625f,
   0.015625f,   0.0625f,   0.09375f,   0.0625f,   0.015625f,
   0.0234375f,   0.09375f,   0.140625f,   0.09375f,   0.0234375f,
   0.015625f,   0.0625f,   0.09375f,   0.0625f,   0.015625f,
   0.00390625f,   0.015625f,   0.0234375f,   0.015625f,   0.00390625f,
};

float2 g_pixelSize = float2(1.f / SCREENWIDTH, 1.f / SCREENHEIGHT);

static const float2 offsets[25] =
{
    float2(-2.0f, -2.0f) * g_pixelSize, float2(-1.0f, -2.0f) * g_pixelSize, float2(0.0f, -2.0f) * g_pixelSize, float2(1.0f, -2.0f) * g_pixelSize, float2(2.0f, -2.0f) * g_pixelSize,
    float2(-2.0f, -1.0f) * g_pixelSize, float2(-1.0f, -1.0f) * g_pixelSize, float2(0.0f, -1.0f) * g_pixelSize, float2(1.0f, -1.0f) * g_pixelSize, float2(2.0f, -1.0f) * g_pixelSize,
    float2(-2.0f, 0.0f) * g_pixelSize, float2(-1.0f, 0.0f) * g_pixelSize,  float2(0.0f, 0.0f) * g_pixelSize,  float2(1.0f, 0.0f) * g_pixelSize,  float2(2.0f, 0.0f) * g_pixelSize,
    float2(-2.0f, 1.0f) * g_pixelSize, float2(-1.0f, 1.0f) * g_pixelSize,  float2(0.0f, 1.0f) * g_pixelSize,  float2(1.0f, 1.0f) * g_pixelSize,  float2(2.0f, 1.0f) * g_pixelSize,
    float2(-2.0f, 2.0f) * g_pixelSize, float2(-1.0f, 2.0f) * g_pixelSize,  float2(0.0f, 2.0f) * g_pixelSize,  float2(1.0f, 2.0f) * g_pixelSize,  float2(2.0f, 2.0f) * g_pixelSize,
};

float getLuminance(float3 RGB)
{
   return log10((RGB.r * 0.3f + RGB.g * 0.59f + RGB.b * 0.11f) + 1.0f) / log10(2.0f);
}

float4 doEdgeAvoidATrousWavelet_improved(float2 uv)
{
   bool needDenoging = false;

   float cum_w = 0.0f;
   float2 UV, step = float2(1.0f / SCREENWIDTH, 1.0f / SCREENHEIGHT);
   float3 RGBtmp, sum = float3(0.0f, 0.0f, 0.0f);

   //if (g_motionBlur.Sample(g_samPoint, uv).w == 0)
   //{
   //    sum = g_colorMap.Load(int4(uv * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0)).xyz;
   //    cum_w = 1;
   //}

   //else
   {
       [unroll(25)] for (int j = 0; j < 25; j++)
       {
           UV = uv + offsets[j];

           RGBtmp = g_motionBlur.Sample(g_samPoint, UV).xyz;
           float searchSuccessRatetmp = g_motionBlur.Sample(g_samPoint, UV).w;

           float l_w = Gaussiankernel[j] * searchSuccessRatetmp;

           float weight = l_w;
           sum += RGBtmp * weight;
           cum_w += weight;
       }
   }

   sum /= cum_w;

   sum = 0.1f;

   return float4(sum, 1.0f);
}

struct FullScreenQuadDenoisingVertexOut
{
   float4 position : SV_POSITION;
   float2 uv : TEXCOORD0;
};

FullScreenQuadDenoisingVertexOut FullScreenQuadDenoisingVS(uint vertexID : SV_VertexID)
{
   FullScreenQuadDenoisingVertexOut OutputVS;

   OutputVS.uv = float2((vertexID << 1) & 2, vertexID & 2);
   OutputVS.position = float4(OutputVS.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

   return OutputVS;
}

float4 FullScreenQuadDenoisingPS(FullScreenQuadDenoisingVertexOut In) : SV_TARGET
{

   //float4 color = doEdgeAvoidATrousWavelet_improved(In.uv);// µð³ëÀÌÂ¡ ÇÑ°Å
   float4 color = g_motionBlur.Sample(g_samLinear, In.uv);// µð³ëÀÌÂ¡ ÇÏ±âÀü

   //float4 color = g_colorMap.Load(int4(In.uv * float2(SCREENWIDTH, SCREENHEIGHT), 1, 0));
   //float4 color = g_motionVectorMap.Load(int4(In.uv * float2(SCREENWIDTH, SCREENHEIGHT), 1, 0));

   //float4 color = g_ColorMap.SampleLevel(g_samLinear, In.uv, 0);
   //float4 color = g_motionVector3DMap4.SampleLevel(g_samLinear, In.uv, 0);
   //float4 color = g_previousDepthMap.SampleLevel(g_samLinear, In.uv, 0)/100;

   return color;
}

technique11 Denoising
{
   pass P0
   {
      //SetVertexShader(CompileShader(vs_5_0, FullScreenQuadDenoisingVS()));
      SetVertexShader(CompileShader(vs_5_0, FSQVS()));

      SetPixelShader(CompileShader(ps_5_0, FullScreenQuadDenoisingPS()));
      SetRasterizerState(rasterizerState_New);
   }

}

