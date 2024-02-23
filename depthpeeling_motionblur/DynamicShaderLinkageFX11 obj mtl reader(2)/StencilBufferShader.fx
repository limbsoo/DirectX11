////#include "function.fx"
////
////Texture2D                         g_modelTexture;
////
////Texture2DMS<uint4, MSAA_LEVEL>         g_color_Depth_motionVectorMap;
////
////Texture2DArray<uint4>               g_sorted_Color_Depth_MotionVectorMap;
////Texture2D                        g_motionVectorMipMap;
////Texture2D                        g_firstLayerColorMap;
////Texture2D                        g_motionVectorMap;
////Texture2D                        g_depthMap;
////
////Texture2D                        g_MotionBlurColorMap;
////Texture2D                        g_MotionBlurDepthWeightMap;
////
//////------------------------------------------------------------- stencil buffer clear pass
////
////struct FSQVSOut
////{
////    float4 position : SV_POSITION;
////    float2 uv   : TEXCOORD0;
////};
////
////FSQVSOut FullScreenQuadVS(uint vertexID : SV_VertexID)
////{
////    FSQVSOut OutputVS;
////
////    OutputVS.uv = float2((vertexID << 1) & 2, vertexID & 2);
////    OutputVS.position = float4(OutputVS.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
////
////    return OutputVS;
////}
////
////technique11 StencilBufferClear
////{
////    pass P0
////    {
////        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
////        SetPixelShader(NULL);
////    }
////}
////
//////------------------------------------------------------------- texture clear pass
////
////uint4 ClearPS(FSQVSOut Input) : SV_TARGET
////{
////   return uint4(0, MAX_DEPTH, 2147450879,  MAX_DEPTH / 2); //2147450879 == ((16bit)0 + (16bit)0), MAX_DEPTH/2 == (32bit)0
////}
////
////technique11 TextureClear
////{
////    pass P0
////    {
////        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
////        SetPixelShader(CompileShader(ps_5_0, ClearPS()));
////        SetRasterizerState(BackCullNoMSAA_RS);
////        SetDepthStencilState(NoDepthNoStencil_DS, 0x00000000);
////        SetBlendState(NoMRT_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
////    }
////}
////
//////------------------------------------------------------------- multi Layer Rendering pass (stencil buffer) 
////
////struct RenderingVSInput
////{
////    float4 Position    : POSITION;
////    float3 Normal      : NORMAL;
////    float2 uv   : TEXCOORD0;
////};
////
////struct RenderingVSOut
////{
////    float4 position : SV_Position;
////    float4 prePosition  : POSITION1;
////    float4 curPosition  : POSITION2;
////    float2 uv : TEXCOORD0;
////};
////
////RenderingVSOut RenderingVS(RenderingVSInput input)
////{
////    RenderingVSOut output;
////
////    output.position = mul(input.Position, g_worldViewProjection);
////    output.prePosition = mul(input.Position, g_previousWorldViewProjection);
////    output.curPosition = mul(input.Position, g_worldViewProjection);
////
////    output.uv = input.uv;
////
////    return output;
////}
////
////uint4 RenderingPS(RenderingVSOut input) : SV_TARGET
////{
////   float3 currentPos = homogenious2uv(input.curPosition);
////   float3 previousPos = homogenious2uv(input.prePosition);
////   float3 motionVector = currentPos - previousPos;
////
////   float3 color = float3(g_modelTexture.Sample(g_samPoint, input.uv).xyz);
////
////   uint packed_Color = pack_rgb(color);
////   uint packed_EyeZ = pack_depth(input.position.w);
////   uint packed_MotionVector_XY = pack_MotionVector_XY(motionVector.xy);
////   uint packed_MotionVector_Z = pack_MotionVector_Z(motionVector.z);
////
////   return uint4(packed_Color, packed_EyeZ, packed_MotionVector_XY, packed_MotionVector_Z);
////}
////
////technique11 Rendering
////{
////    pass P0
////    {
////        SetRasterizerState(BackCullNoMSAA_RS);
////        SetVertexShader(CompileShader(vs_5_0, RenderingVS()));
////        SetPixelShader(CompileShader(ps_5_0, RenderingPS()));
////        SetBlendState(NoMRT_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
////    }
////}
////
//////------------------------------------------------------------- First Layer Rendering pass
////
////struct motionVectorMipMap_colorMap_outPut
////{
////    float4 MotionVectorMipMap;
////    float4 ColorMap;
////    float4 MotionVectorMap;
////    float4 depthMap;
////};
////
////RenderingVSOut motionVectorMipMap_colorMap_renderingVS(RenderingVSInput Input)
////{
////    RenderingVSOut OutputVS;
////
////    OutputVS.position = mul(Input.Position, g_worldViewProjection);
////    OutputVS.prePosition = mul(Input.Position, g_previousWorldViewProjection);
////    OutputVS.curPosition = mul(Input.Position, g_worldViewProjection);
////
////    OutputVS.uv = Input.uv;
////
////    return OutputVS;
////}
////
////motionVectorMipMap_colorMap_outPut motionVectorMipMap_colorMap_renderingPS(RenderingVSOut input) : SV_TARGET
////{
////   motionVectorMipMap_colorMap_outPut output;
////
////   float3 currentPos = homogenious2uv(input.curPosition);
////   float3 previousPos = homogenious2uv(input.prePosition);
////   float3 motionVector = currentPos - previousPos;
////
////   output.MotionVectorMipMap = float4(motionVector.xy, motionVector.xy * motionVector.xy);
////   output.ColorMap = g_modelTexture.Sample(g_samPoint, input.uv);
////   output.MotionVectorMap = float4(motionVector, 0);
////   output.depthMap = float4(input.position.w, input.position.w, input.position.w, 0);
////
////   return output;
////}
////
////technique11 motionVectorMipMap_colorMap_Rendering
////{
////    pass P0
////    {
////        SetRasterizerState(NoCullNoMSAA_RS);
////        //SetRasterizerState(BackCullNoMSAA_RS);
////        SetVertexShader(CompileShader(vs_5_0, motionVectorMipMap_colorMap_renderingVS()));
////        SetPixelShader(CompileShader(ps_5_0, motionVectorMipMap_colorMap_renderingPS()));
////    }
////}
////
//////------------------------------------------------------------- sorting pass
////
////struct FSQVSInput
////{
////    float4 Position    : POSITION;
////    float3 Normal      : NORMAL;
////    float2 uv   : TEXCOORD0;
////};
////
////FSQVSOut FSQVS(FSQVSInput input)
////{
////    FSQVSOut OutputVS;
////
////    OutputVS.position = input.Position;
////    OutputVS.uv = input.uv;
////    return OutputVS;
////}
////
////struct sortPSOutPut
////{
////    uint4 sorted_Layer_0;
////    uint4 sorted_Layer_1;
////    uint4 sorted_Layer_2;
////    uint4 sorted_Layer_3;
////    uint4 sorted_Layer_4;
////    uint4 sorted_Layer_5;
////    uint4 sorted_Layer_6;
////    uint4 sorted_Layer_7;
////};
////
////sortPSOutPut SortingPS(FSQVSOut input) : SV_TARGET
////{
////   uint4 frag[MSAA_LEVEL];
////   [unroll] for (int i = 0; i < MSAA_LEVEL; ++i) {
////      frag[i] = g_color_Depth_motionVectorMap.Load(input.position.xy, i);
////   }
////   BitonicSortF2B(frag, MSAA_LEVEL);
////
////   float3 color = g_firstLayerColorMap.SampleLevel(g_samLinear, input.uv, 0).xyz;
////   float3 motionVector = g_motionVectorMap.SampleLevel(g_samLinear, input.uv, 0).xyz;
////   float dpeth = g_depthMap.SampleLevel(g_samLinear, input.uv, 0).x;
////
////   uint packed_Color = pack_rgb(color);
////   uint packed_EyeZ = pack_depth(dpeth);
////   uint packed_MotionVector_XY = pack_MotionVector_XY(motionVector.xy);
////   uint packed_MotionVector_Z = pack_MotionVector_Z(motionVector.z);
////
////   sortPSOutPut output;
////   output.sorted_Layer_0 = uint4(packed_Color, packed_EyeZ, packed_MotionVector_XY, packed_MotionVector_Z);
////   //output.sorted_Layer_0 = frag[0];
////   output.sorted_Layer_1 = frag[1];
////   output.sorted_Layer_2 = frag[2];
////   output.sorted_Layer_3 = frag[3];
////   output.sorted_Layer_4 = frag[4];
////   output.sorted_Layer_5 = frag[5];
////   output.sorted_Layer_6 = frag[6];
////   output.sorted_Layer_7 = frag[7];
////
////   return output;
////}
////
////technique11 Sorting
////{
////    pass P0
////    {
////        SetRasterizerState(NoCullNoMSAA_RS);
////        SetDepthStencilState(NoDepthNoStencil_DS, 0x00000000);
////        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
////        SetPixelShader(CompileShader(ps_5_0, SortingPS()));
////        SetBlendState(NoMRT_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
////    }
////}
////
//////------------------------------------------------------------- motion blur pass
////
////
////
////struct motionBlurPSOutput
////{
////    float4 color;
////    float4 dpeth_successRate_motionVector;
////};
////
////float4 searchBoundary(float2 uv)
////{
////    const int constant = 3;
////
////    float2 mean = g_motionVectorMipMap.SampleLevel(g_samLinear, uv, MIPMAP_LEVEL).xy;
////    float2 squaMean = g_motionVectorMipMap.SampleLevel(g_samLinear, uv, MIPMAP_LEVEL).zw;
////
////    float2 standardDeviation = sqrt(squaMean - mean * mean);
////    float2 Max = mean + standardDeviation * constant;
////    float2 Min = mean - standardDeviation * constant;
////
////    float2 leftTopCorner = max(uv - Max, float2(0.0f, 0.0f));
////    float2 rightBottomCorner = min(uv - Min, float2(1.0f, 1.0f));
////
////    uv = (leftTopCorner + rightBottomCorner) / 2;
////
////    mean = g_motionVectorMipMap.SampleLevel(g_samLinear, uv, MIPMAP_LEVEL).xy;
////    squaMean = g_motionVectorMipMap.SampleLevel(g_samLinear, uv, MIPMAP_LEVEL).zw;
////
////    standardDeviation = sqrt(squaMean - mean * mean);
////    Max = mean + standardDeviation * constant;
////    Min = mean - standardDeviation * constant;
////
////    leftTopCorner = max(uv - Max, float2(0.0f, 0.0f));
////    rightBottomCorner = min(uv - Min, float2(1.0f, 1.0f));
////
////    return float4(leftTopCorner, rightBottomCorner);
////}
////
////motionBlurPSOutput findLineAndGetColor(float2 uv)
////{
////    static const float epsilon = g_pixelSize.y * EPSILON;
////    float2 randomUV = float2(0, 0);
////
////    float2 intersecting_Motion_Vector[N_RANDOMPICK * N_LAYER];
////    int nNumintersecting_Motion_Vector = 1;
////    uint4 firstLayer = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(uv), 0, 0));
////    intersecting_Motion_Vector[0] = unpack_MotionVector_XY(firstLayer.z);
////
////    float4 motionBoundary = searchBoundary(uv);
////    motionBlurPSOutput output;
////
////    for (int sampleCnt = 0; sampleCnt < N_RANDOMPICK; sampleCnt++) {
////
////        randomUV = float2(Random(float2(uv.x + sampleCnt * 0.0002468f, uv.y + sampleCnt * 0.0003456f)), Random(float2(uv.x + sampleCnt * 0.0001357f, uv.y + sampleCnt * 0.0004567f)));
////        float2 currentCandidate = float2(motionBoundary.x + randomUV.x * abs(motionBoundary.z - motionBoundary.x), motionBoundary.y + randomUV.y * abs(motionBoundary.w - motionBoundary.y));
////
////        for (int Layer = 0; Layer < N_LAYER; Layer++) {
////            uint4 packed_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(currentCandidate), Layer, 0));
////            float2 candidate_MotionVector = unpack_MotionVector_XY(packed_Color_Depth_MotionVector.z);
////
////            bool isFound = false;
////            for (int j = 0; j < N_SEARCH; j++) {
////
////                if (intersect(uv, candidate_MotionVector, currentCandidate, epsilon)) {
////                    isFound = true;
////                    break;
////                }
////
////                currentCandidate = uv - candidate_MotionVector * sampleCnt / N_RANDOMPICK;
////
////                uint4 packed_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(currentCandidate), Layer, 0));
////                candidate_MotionVector = unpack_MotionVector_XY(packed_Color_Depth_MotionVector.z);
////            }
////
////            if (isFound) {
////                int k;
////                for (k = 0; k < nNumintersecting_Motion_Vector; k++) {
////                    if (distance(candidate_MotionVector, intersecting_Motion_Vector[k]) < g_pixelSize.y) break;
////                }
////
////                if (k == nNumintersecting_Motion_Vector) {
////                    intersecting_Motion_Vector[k] = candidate_MotionVector;
////                    nNumintersecting_Motion_Vector++;
////                }
////            }
////        }
////    }
////
////    float4 returnColor = float4(unpack_rgb(firstLayer.x), 0);
////    float sumDepth = 0;
////
////    if (nNumintersecting_Motion_Vector == 1 && -g_pixelSize.x < intersecting_Motion_Vector[0].x && intersecting_Motion_Vector[0].x < g_pixelSize.x && -g_pixelSize.y < intersecting_Motion_Vector[0].y && intersecting_Motion_Vector[0].y < g_pixelSize.y) {
////        output.color = float4(unpack_rgb(firstLayer.x), 0);
////        output.dpeth_successRate_motionVector = float4(unpack_depth(firstLayer.y), 1, 0, 0);
////    }
////    else {
////        bool searchFirstLayer = false;
////        float nNumOfSucess = 0;
////        float3 colorSum = float3(0.0f, 0.0f, 0.0f);
////        for (int time = 0; time < N_SAMPLINGTIME; time++) {
////            randomUV = float2(Random(float2(uv.x + time * 0.0002468f, uv.y + time * 0.0003456f)), Random(float2(uv.x + time * 0.0001357f, uv.y + time * 0.0004567f)));
////            float targetDepth = ZFAR;
////            float3 targetColor = float3(-1.f, 0.f, 0.f);
////            for (int j = 0; j < nNumintersecting_Motion_Vector; j++) {
////                for (int layer = 0; layer < N_LAYER; layer++) {
////                    float2 inverseMotionVector = uv - intersecting_Motion_Vector[j];
////
////                    float clipped_ratio = MOTIONVECTOR_MAX;
////                    float ratio = 1.f;
////
////                    if (inverseMotionVector.x < 0.f) {
////                        if (clipped_ratio > uv.x)
////                            clipped_ratio = abs(uv.x / intersecting_Motion_Vector[j].x);
////                    }
////                    else if (inverseMotionVector.x > 1.f) {
////                        if (clipped_ratio > 1.f - uv.x)
////                            clipped_ratio = (1.f - uv.x) / abs(intersecting_Motion_Vector[j].x);
////                    }
////
////                    if (inverseMotionVector.y < 0.f) {
////                        if (clipped_ratio > uv.y)
////                            clipped_ratio = abs(uv.y / intersecting_Motion_Vector[j].y);
////                    }
////                    else if (inverseMotionVector.y > 1.f) {
////                        if (clipped_ratio > 1.f - uv.y)
////                            clipped_ratio = (1.f - uv.y) / abs(intersecting_Motion_Vector[j].y);
////                    }
////
////                    if (clipped_ratio != MOTIONVECTOR_MAX)   ratio = clipped_ratio;
////
////                    float2 Motion_Vector_Sample_Pos = uv - intersecting_Motion_Vector[j] * ratio * (time + randomUV) / N_SAMPLINGTIME;
////
////
////                    uint4 pos_Sample_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(Motion_Vector_Sample_Pos), layer, 0));
////                    float3 pos_Sample_MotionVector = float3(unpack_MotionVector_XY(pos_Sample_Color_Depth_MotionVector.z), unpack_MotionVector_Z(pos_Sample_Color_Depth_MotionVector.w));
////
////                    if (distance(intersecting_Motion_Vector[j], pos_Sample_MotionVector.xy) < g_pixelSize.y) {
////                        pos_Sample_MotionVector.z *= (float)(time + randomUV) / N_SAMPLINGTIME;
////                        float pos_Sample_Depth = unpack_depth(pos_Sample_Color_Depth_MotionVector.y);
////
////                        if (pos_Sample_MotionVector.z + pos_Sample_Depth < targetDepth) {
////                            if (layer == 0) searchFirstLayer = true;
////                            sumDepth += pos_Sample_Depth;
////                            targetColor = unpack_rgb(pos_Sample_Color_Depth_MotionVector.x);
////                            targetDepth = pos_Sample_MotionVector.z + pos_Sample_Depth;
////                            break;
////                        }
////                    }
////                }
////            }
////
////            if (targetColor.x > -1.f) {
////                colorSum += targetColor;
////                nNumOfSucess++;
////            }
////        }
////
////        float successRate = nNumOfSucess / N_SAMPLINGTIME;
////        float3 resColor = unpack_rgb(firstLayer.x);
////
////        if (nNumOfSucess > 1) resColor = colorSum / nNumOfSucess;
////        //if (!searchFirstLayer) successRate = 0;
////        output.color = float4(resColor, 1);
////        output.dpeth_successRate_motionVector = float4(sumDepth / nNumOfSucess, successRate * successRate, unpack_MotionVector_XY(firstLayer.z));
////    }
////
////    return output;
////}
////
////motionBlurPSOutput MakeMotionBlurPS(FSQVSOut input) : SV_TARGET
////{
////   return findLineAndGetColor(input.uv);
////}
////
////technique11 MakeMotionBlur
////{
////    pass P0
////    {
////        SetRasterizerState(NoCullNoMSAA_RS);
////        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
////        SetPixelShader(CompileShader(ps_5_0, MakeMotionBlurPS()));
////    }
////}
////
//////------------------------------------------------------------- denoising pass
////
////float4 denoising(float2 uv)
////{
////    bool needDenoging = false;
////    float sum_w = 0, depthCmp = 0.9;
////    float2 aroundUV;
////    float3 sumColor = float3(0.0f, 0.0f, 0.0f), resColor;
////    float4 aroundDepth_SuccessRate_motionVector;
////    uint4 firstLayer = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(uv), 0, 0));
////
////    [unroll(25)] for (int j = 0; j < 25; j++)
////    {
////        aroundUV = uv + offsets[j];
////        float3 RGBtmp = g_MotionBlurColorMap.Sample(g_samLinear, aroundUV).xyz;
////        aroundDepth_SuccessRate_motionVector = g_MotionBlurDepthWeightMap.Sample(g_samLinear, aroundUV);
////        float d_w = max((1 - abs(unpack_depth(firstLayer.y) - aroundDepth_SuccessRate_motionVector.x) / ZFAR), 0);
////        if (d_w < depthCmp) d_w = 0.3;
////
////        float weight = gaussianKernel[j] * aroundDepth_SuccessRate_motionVector.y /** d_w*/;
////        sumColor += RGBtmp * weight;
////        sum_w += weight;
////        if (needDenoging == false && aroundDepth_SuccessRate_motionVector.z != 0.f && aroundDepth_SuccessRate_motionVector.w != 0.f) needDenoging = true;
////    }
////    resColor = sumColor / sum_w;
////    if (!needDenoging) resColor = unpack_rgb(firstLayer.x);
////
////    return float4(resColor, 1);
////}
////
////float4 DenoisingPS(FSQVSOut input) : SV_TARGET
////{
////   float4 color;
//////uint4 pos_Sample_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(input.uv), 0, 0));
//////color = float4(unpack_rgb(pos_Sample_Color_Depth_MotionVector.x), 0.f);
//////color = float4(abs(unpack_MotionVector_XY(pos_Sample_Color_Depth_MotionVector.z)), abs(unpack_MotionVector_Z(pos_Sample_Color_Depth_MotionVector.w)), 0.f);
//////color = float4(color.x * color.x, color.y* color.y, color.z * color.z, 0.f);
//////color = float4(unpack_depth(pos_Sample_Color_Depth_MotionVector.y) / ZFAR, unpack_depth(pos_Sample_Color_Depth_MotionVector.y) / ZFAR, unpack_depth(pos_Sample_Color_Depth_MotionVector.y) / ZFAR,0);
////
//////color = float4(g_motionVectorMipMap.SampleLevel(g_samPoint, input.uv, 0).xy, 0, 0);
//////color = g_firstLayerColorMap.SampleLevel(g_samLinear, input.uv, 0);
//////color = g_motionVectorMap.SampleLevel(g_samLinear, input.uv, 0);
//////color = g_depthMap.SampleLevel(g_samLinear, input.uv, 0);
////
//////color = g_MotionBlurColorMap.Sample(g_samLinear, input.uv);
//////color = float4(g_MotionBlurColorMap.Sample(g_samLinear, input.uv).w / 100, g_MotionBlurColorMap.Sample(g_samLinear, input.uv).w / 100, g_MotionBlurColorMap.Sample(g_samLinear, input.uv).w / 100, 1);
////
////
////
//////color = float4(denoising(input.uv).y, denoising(input.uv).y, denoising(input.uv).y ,0);
////
//////float depth = g_MotionBlurDepthWeightMap.Sample(g_samLinear, input.uv).y;
//////color = float4(depth / ZFAR, depth/ ZFAR, depth / ZFAR, 0);
////
//////float successRate = g_MotionBlurColorMap.Sample(g_samLinear, input.uv).w;
//////color = float4(successRate, successRate, successRate, 0);
////
//////color = float4(color.w, color.w, color.w, 1);
//////color = float4(input.uv, 0, 1);
////
////color = denoising(input.uv);
////
////return color;
////}
////
////technique11 Denoising
////{
////    pass P0
////    {
////        SetRasterizerState(NoCullNoMSAA_RS);
////        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
////        SetPixelShader(CompileShader(ps_5_0, DenoisingPS()));
////    }
////}




//#include "function.fx"

//Texture2D g_modelTexture;

//Texture2DMS<uint4, MSAA_LEVEL> g_color_Depth_motionVectorMap;

//Texture2DArray<uint4> g_sorted_Color_Depth_MotionVectorMap;
//Texture2D g_motionVectorMipMap;
//Texture2D g_firstLayerColorMap;
//Texture2D g_motionVectorMap;
//Texture2D g_depthMap;

//Texture2D g_MotionBlurColorMap;
//Texture2D g_MotionBlurDepthWeightMap;

////------------------------------------------------------------- stencil buffer clear pass

//struct FSQVSOut
//{
//    float4 position : SV_POSITION;
//    float2 uv : TEXCOORD0;
//};

//FSQVSOut FullScreenQuadVS(uint vertexID : SV_VertexID)
//{
//    FSQVSOut OutputVS;

//    OutputVS.uv = float2((vertexID << 1) & 2, vertexID & 2);
//    OutputVS.position = float4(OutputVS.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

//    return OutputVS;
//}

//technique11 StencilBufferClear
//{
//    pass P0
//    {
//        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
//        SetPixelShader(NULL);
//    }
//}

////------------------------------------------------------------- texture clear pass

//uint4 ClearPS(FSQVSOut Input) : SV_TARGET
//{
//    return uint4(0, MAX_DEPTH, 2147450879, MAX_DEPTH / 2); //2147450879 == ((16bit)0 + (16bit)0), MAX_DEPTH/2 == (32bit)0
//}

//technique11 TextureClear
//{
//    pass P0
//    {
//        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
//        SetPixelShader(CompileShader(ps_5_0, ClearPS()));
//        SetRasterizerState(BackCullNoMSAA_RS);
//        SetDepthStencilState(NoDepthNoStencil_DS, 0x00000000);
//        SetBlendState(NoMRT_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
//    }
//}

////------------------------------------------------------------- multi Layer Rendering pass (stencil buffer) 

//struct RenderingVSInput
//{
//    float4 Position : POSITION;
//    float3 Normal : NORMAL;
//    float2 uv : TEXCOORD0;
//};

//struct RenderingVSOut
//{
//    float4 position : SV_Position;
//    float4 prePosition : POSITION1;
//    float4 curPosition : POSITION2;
//    float2 uv : TEXCOORD0;

//};

//struct RenderingVSOut1
//{
//    float4 position : SV_Position;
//    float4 prePosition : POSITION1;
//    float4 curPosition : POSITION2;
//    float2 uv : TEXCOORD0;

//    float3 Normal : NORMAL;
//    float4 WorldPos : POSITION3;
//};

//RenderingVSOut1 RenderingVS(RenderingVSInput input)
//{
//    RenderingVSOut1 output;

//    output.position = mul(input.Position, g_worldViewProjection);
//    output.prePosition = mul(input.Position, g_previousWorldViewProjection);
//    output.curPosition = mul(input.Position, g_worldViewProjection);

//    output.uv = input.uv;

//    output.WorldPos = mul(input.Position, g_world);
//    output.Normal = mul(float4(input.Normal, 0.f), g_world).xyz;
//    output.Normal = normalize(input.Normal);

//    return output;
//}

//uint4 RenderingPS(RenderingVSOut1 input) : SV_TARGET
//{
//    float3 currentPos = homogenious2uv(input.curPosition);
//    float3 previousPos = homogenious2uv(input.prePosition);
//    float3 motionVector = currentPos - previousPos;

//    float3 color = float3(g_modelTexture.Sample(g_samPoint, input.uv).xyz);



//    float3 ambientLightColor = float3(0.1f, 0.1f, 0.1f);
//    float ambientLightStrength = 0.01f;
//    float3 dynamicLightPosition = float3(15.f, -6.f, 16.f);
//    float dynamicLightStrength = 0.9f;
//    float3 dynamicLightColor = float3(1.f, 1.f, 1.f);

//    float3 ambientLight = ambientLightColor * ambientLightStrength;
//    float3 appliedLight = ambientLight;
//    float3 vectorToLight = normalize(dynamicLightPosition - input.WorldPos.xyz).xyz;
//    float3 diffuseLightIntensity = max(dot(vectorToLight, input.Normal), 0);
//    float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;
//    appliedLight += diffuseLight;
//    color = color.xyz * appliedLight;
//   //outputPS.color = float4(finalColor, depth);



//    uint packed_Color = pack_rgb(color);
//    uint packed_EyeZ = pack_depth(input.position.w);
//    uint packed_MotionVector_XY = pack_MotionVector_XY(motionVector.xy);
//    uint packed_MotionVector_Z = pack_MotionVector_Z(motionVector.z);

//    return uint4(packed_Color, packed_EyeZ, packed_MotionVector_XY, packed_MotionVector_Z);
//}

//technique11 Rendering
//{
//    pass P0
//    {
//        SetRasterizerState(BackCullNoMSAA_RS);
//        SetVertexShader(CompileShader(vs_5_0, RenderingVS()));
//        SetPixelShader(CompileShader(ps_5_0, RenderingPS()));
//        SetBlendState(NoMRT_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
//    }
//}

////------------------------------------------------------------- First Layer Rendering pass

//struct motionVectorMipMap_colorMap_outPut
//{
//    float4 MotionVectorMipMap;
//    float4 ColorMap;
//    float4 MotionVectorMap;
//    float4 depthMap;
//};

//RenderingVSOut1 motionVectorMipMap_colorMap_renderingVS(RenderingVSInput Input)
//{
//    RenderingVSOut1 OutputVS;

//    OutputVS.position = mul(Input.Position, g_worldViewProjection);
//    OutputVS.prePosition = mul(Input.Position, g_previousWorldViewProjection);
//    OutputVS.curPosition = mul(Input.Position, g_worldViewProjection);

//    OutputVS.uv = Input.uv;

//    OutputVS.WorldPos = mul(Input.Position, g_world);
//    OutputVS.Normal = mul(float4(Input.Normal, 0.f), g_world).xyz;
//    OutputVS.Normal = normalize(Input.Normal);

//    return OutputVS;
//}

//motionVectorMipMap_colorMap_outPut motionVectorMipMap_colorMap_renderingPS(RenderingVSOut1 input) : SV_TARGET
//{
//    motionVectorMipMap_colorMap_outPut output;

//    float3 currentPos = homogenious2uv(input.curPosition);
//    float3 previousPos = homogenious2uv(input.prePosition);
//    float3 motionVector = currentPos - previousPos;

//    output.MotionVectorMipMap = float4(motionVector.xy, motionVector.xy * motionVector.xy);
//    output.ColorMap = g_modelTexture.Sample(g_samPoint, input.uv);
//    output.MotionVectorMap = float4(motionVector, 0);
//    output.depthMap = float4(input.position.w, input.position.w, input.position.w, 0);


//    float3 ambientLightColor = float3(0.1f, 0.1f, 0.1f);
//    float ambientLightStrength = 0.01f;
//    float3 dynamicLightPosition = float3(15.f, -6.f, 16.f);
//    float dynamicLightStrength = 0.9f;
//    float3 dynamicLightColor = float3(1.f, 1.f, 1.f);

//    float3 ambientLight = ambientLightColor * ambientLightStrength;
//    float3 appliedLight = ambientLight;
//    float3 vectorToLight = normalize(dynamicLightPosition - input.WorldPos.xyz).xyz;
//    float3 diffuseLightIntensity = max(dot(vectorToLight, input.Normal), 0);
//    float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;
//    appliedLight += diffuseLight;
//    output.ColorMap = float4(output.ColorMap.xyz * appliedLight, 1);


//    return output;
//}

//technique11 motionVectorMipMap_colorMap_Rendering
//{
//    pass P0
//    {
//        SetRasterizerState(NoCullNoMSAA_RS);
//        //SetRasterizerState(BackCullNoMSAA_RS);
//        SetVertexShader(CompileShader(vs_5_0, motionVectorMipMap_colorMap_renderingVS()));
//        SetPixelShader(CompileShader(ps_5_0, motionVectorMipMap_colorMap_renderingPS()));
//    }
//}

////------------------------------------------------------------- sorting pass

//struct FSQVSInput
//{
//    float4 Position : POSITION;
//    float3 Normal : NORMAL;
//    float2 uv : TEXCOORD0;
//};

//FSQVSOut FSQVS(FSQVSInput input)
//{
//    FSQVSOut OutputVS;

//    OutputVS.position = input.Position;
//    OutputVS.uv = input.uv;
//    return OutputVS;
//}

//struct sortPSOutPut
//{
//    uint4 sorted_Layer_0;
//    uint4 sorted_Layer_1;
//    uint4 sorted_Layer_2;
//    uint4 sorted_Layer_3;
//    uint4 sorted_Layer_4;
//    uint4 sorted_Layer_5;
//    uint4 sorted_Layer_6;
//    uint4 sorted_Layer_7;
//};

//sortPSOutPut SortingPS(FSQVSOut input) : SV_TARGET
//{
//    uint4 frag[MSAA_LEVEL];
//   [unroll]
//    for (int i = 0; i < MSAA_LEVEL; ++i)
//    {
//        frag[i] = g_color_Depth_motionVectorMap.Load(input.position.xy, i);
//    }
//    BitonicSortF2B(frag, MSAA_LEVEL);

//    float3 color = g_firstLayerColorMap.SampleLevel(g_samLinear, input.uv, 0).xyz;
//    float3 motionVector = g_motionVectorMap.SampleLevel(g_samLinear, input.uv, 0).xyz;
//    float dpeth = g_depthMap.SampleLevel(g_samLinear, input.uv, 0).x;

//    uint packed_Color = pack_rgb(color);
//    uint packed_EyeZ = pack_depth(dpeth);
//    uint packed_MotionVector_XY = pack_MotionVector_XY(motionVector.xy);
//    uint packed_MotionVector_Z = pack_MotionVector_Z(motionVector.z);

//    sortPSOutPut output;
//   //output.sorted_Layer_0 = uint4(packed_Color, packed_EyeZ, packed_MotionVector_XY, packed_MotionVector_Z);

//   //output.sorted_Layer_0 = float4(g_color_Depth_motionVectorMap.SampleLevel(g_samLinear, input.uv, 0).xyz, 1);
//    output.sorted_Layer_0 = frag[0];

//    output.sorted_Layer_1 = frag[1];
//    output.sorted_Layer_2 = frag[2];
//    output.sorted_Layer_3 = frag[3];
//    output.sorted_Layer_4 = frag[4];
//    output.sorted_Layer_5 = frag[5];
//    output.sorted_Layer_6 = frag[6];
//    output.sorted_Layer_7 = frag[7];

//    return output;
//}

//technique11 Sorting
//{
//    pass P0
//    {
//        SetRasterizerState(NoCullNoMSAA_RS);
//        SetDepthStencilState(NoDepthNoStencil_DS, 0x00000000);
//        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
//        SetPixelShader(CompileShader(ps_5_0, SortingPS()));
//        SetBlendState(NoMRT_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
//    }
//}

////------------------------------------------------------------- motion blur pass



//struct motionBlurPSOutput
//{
//    float4 color;
//    float4 dpeth_successRate_motionVector;
//};

//float4 searchBoundary(float2 uv)
//{
//    const int constant = 3;

//    float2 mean = g_motionVectorMipMap.SampleLevel(g_samLinear, uv, MIPMAP_LEVEL).xy;
//    float2 squaMean = g_motionVectorMipMap.SampleLevel(g_samLinear, uv, MIPMAP_LEVEL).zw;

//    float2 standardDeviation = sqrt(squaMean - mean * mean);
//    float2 Max = mean + standardDeviation * constant;
//    float2 Min = mean - standardDeviation * constant;

//    float2 leftTopCorner = max(uv - Max, float2(0.0f, 0.0f));
//    float2 rightBottomCorner = min(uv - Min, float2(1.0f, 1.0f));

//    return float4(leftTopCorner, rightBottomCorner);
//}


//motionBlurPSOutput findLineAndGetColor(float2 uv)
//{
//    static const float epsilon = g_pixelSize.y * EPSILON;
//    float2 randomUV = float2(0, 0);

//    float2 intersecting_Motion_Vector[N_RANDOMPICK * N_LAYER];
//    int nNumintersecting_Motion_Vector = 1;
//    uint4 firstLayer = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(uv), 0, 0));
//    intersecting_Motion_Vector[0] = unpack_MotionVector_XY(firstLayer.z);

//    float4 motionBoundary = searchBoundary(uv);
//    motionBlurPSOutput output;

//    for (int sampleCnt = 0; sampleCnt < N_RANDOMPICK; sampleCnt++)
//    {

//        randomUV = float2(Random(float2(uv.x + sampleCnt * 0.0002468f, uv.y + sampleCnt * 0.0003456f)), Random(float2(uv.x + sampleCnt * 0.0001357f, uv.y + sampleCnt * 0.0004567f)));
//        float2 currentCandidate = float2(motionBoundary.x + randomUV.x * abs(motionBoundary.z - motionBoundary.x), motionBoundary.y + randomUV.y * abs(motionBoundary.w - motionBoundary.y));

//        for (int Layer = 0; Layer < N_LAYER; Layer++)
//        {
//            uint4 packed_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(currentCandidate), Layer, 0));
//            float2 candidate_MotionVector = unpack_MotionVector_XY(packed_Color_Depth_MotionVector.z);

//            bool isFound = false;
//            for (int j = 0; j < N_SEARCH; j++)
//            {

//                if (intersect(uv, candidate_MotionVector, currentCandidate, epsilon))
//                {
//                    isFound = true;
//                    break;
//                }

//                currentCandidate = uv - candidate_MotionVector * sampleCnt / N_RANDOMPICK;

//                uint4 packed_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(currentCandidate), Layer, 0));
//                candidate_MotionVector = unpack_MotionVector_XY(packed_Color_Depth_MotionVector.z);
//            }

//            if (isFound)
//            {
//                int k;
//                for (k = 0; k < nNumintersecting_Motion_Vector; k++)
//                {
//                    if (distance(candidate_MotionVector, intersecting_Motion_Vector[k]) < g_pixelSize.y)
//                        break;
//                }

//                if (k == nNumintersecting_Motion_Vector)
//                {
//                    intersecting_Motion_Vector[k] = candidate_MotionVector;
//                    nNumintersecting_Motion_Vector++;
//                }
//            }
//        }
//    }


//    float sumDepth = 0;

//    if (nNumintersecting_Motion_Vector == 1 && -g_pixelSize.x < intersecting_Motion_Vector[0].x && intersecting_Motion_Vector[0].x < g_pixelSize.x && -g_pixelSize.y < intersecting_Motion_Vector[0].y && intersecting_Motion_Vector[0].y < g_pixelSize.y)
//    {
//        output.color = float4(unpack_rgb(firstLayer.x), 0);
//        output.dpeth_successRate_motionVector = float4(unpack_depth(firstLayer.y), 1, 0, 0);
//    }
//    else
//    {
//        bool searchFirstLayer = false;
//        float nNumOfSucess = 0;
//        float3 colorSum = float3(0.0f, 0.0f, 0.0f);
//        for (int time = 0; time < N_SAMPLINGTIME; time++)
//        {
//            randomUV = float2(Random(float2(uv.x + time * 0.0002468f, uv.y + time * 0.0003456f)), Random(float2(uv.x + time * 0.0001357f, uv.y + time * 0.0004567f)));
//            float targetDepth = ZFAR;
//            float3 targetColor = float3(-1.f, 0.f, 0.f);
//            for (int j = 0; j < nNumintersecting_Motion_Vector; j++)
//            {
//                for (int layer = 0; layer < N_LAYER; layer++)
//                {
//                    float2 inverseMotionVector = uv - intersecting_Motion_Vector[j];

//                    float clipped_ratio = MOTIONVECTOR_MAX;
//                    float ratio = 1.f;

//                    if (inverseMotionVector.x < 0.f)
//                    {
//                        if (clipped_ratio > uv.x)
//                            clipped_ratio = abs(uv.x / intersecting_Motion_Vector[j].x);
//                    }
//                    else if (inverseMotionVector.x > 1.f)
//                    {
//                        if (clipped_ratio > 1.f - uv.x)
//                            clipped_ratio = (1.f - uv.x) / abs(intersecting_Motion_Vector[j].x);
//                    }

//                    if (inverseMotionVector.y < 0.f)
//                    {
//                        if (clipped_ratio > uv.y)
//                            clipped_ratio = abs(uv.y / intersecting_Motion_Vector[j].y);
//                    }
//                    else if (inverseMotionVector.y > 1.f)
//                    {
//                        if (clipped_ratio > 1.f - uv.y)
//                            clipped_ratio = (1.f - uv.y) / abs(intersecting_Motion_Vector[j].y);
//                    }

//                    if (clipped_ratio != MOTIONVECTOR_MAX)
//                        ratio = clipped_ratio;

//                    float2 Motion_Vector_Sample_Pos = uv - intersecting_Motion_Vector[j] * ratio * (time + randomUV) / N_SAMPLINGTIME;


//                    uint4 pos_Sample_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(Motion_Vector_Sample_Pos), layer, 0));
//                    float3 pos_Sample_MotionVector = float3(unpack_MotionVector_XY(pos_Sample_Color_Depth_MotionVector.z), unpack_MotionVector_Z(pos_Sample_Color_Depth_MotionVector.w));

//                    if (distance(intersecting_Motion_Vector[j], pos_Sample_MotionVector.xy) < g_pixelSize.y)
//                    {
//                        pos_Sample_MotionVector.z *= (float) (time + randomUV) / N_SAMPLINGTIME;
//                        float pos_Sample_Depth = unpack_depth(pos_Sample_Color_Depth_MotionVector.y);

//                        if (pos_Sample_MotionVector.z + pos_Sample_Depth < targetDepth)
//                        {
//                            if (layer == 0)
//                                searchFirstLayer = true;
//                            sumDepth += pos_Sample_Depth;
//                            targetColor = unpack_rgb(pos_Sample_Color_Depth_MotionVector.x);
//                            targetDepth = pos_Sample_MotionVector.z + pos_Sample_Depth;
//                            break;
//                        }
//                    }
//                }
//            }

//            if (targetColor.x >= 0.f)
//            {
//                colorSum += targetColor;
//                nNumOfSucess++;
//            }
//        }

//        float successRate = 0;
//        float3 resColor = unpack_rgb(firstLayer.x);

//        if (nNumOfSucess > 0)
//        {
//            resColor = colorSum / nNumOfSucess;
//            successRate = nNumOfSucess / N_SAMPLINGTIME;
//        }
//        if (!searchFirstLayer)
//        {
//            successRate = 0.001;
//        }
//        output.color = float4(resColor, 1);
//        output.dpeth_successRate_motionVector = float4(sumDepth / nNumOfSucess, successRate * successRate, unpack_MotionVector_XY(firstLayer.z));
//    }

//    return output;
//}

////motionBlurPSOutput findLineAndGetColor1(float2 uv)
////{
////    motionBlurPSOutput output;
////
////    float4 pos_Sample_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(uv * float2(SCREENWIDTH, SCREENHEIGHT), 0, 0)).xyzw;
////
////    //float4 pos_Sample_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(uv), 0, 0));
////    //float3 targetColor = float3(1,1,0);
////    //float3 targetColor = pos_Sample_Color_Depth_MotionVector.xyz;
////
////
////    //float3 targetColor = unpack_rgb(pos_Sample_Color_Depth_MotionVector.x);
////    //float3 targetColor = g_firstLayerColorMap.SampleLevel(g_samLinear, uv, 0).xyz;
////
////    output.color = float4(targetColor, 1);
////    output.dpeth_successRate_motionVector = float4(1,1,1,1);
////    return output;
////}

//motionBlurPSOutput MakeMotionBlurPS(FSQVSOut input) : SV_TARGET
//{
//    return findLineAndGetColor(input.uv);
//   //return findLineAndGetColor1(input.uv);
//}

//technique11 MakeMotionBlur
//{
//    pass P0
//    {
//        SetRasterizerState(NoCullNoMSAA_RS);
//        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
//        SetPixelShader(CompileShader(ps_5_0, MakeMotionBlurPS()));
//    }
//}

////------------------------------------------------------------- denoising pass

//float4 denoising(float2 uv)
//{
//    bool needDenoging = false;
//    float sum_w = 0, depthCmp = 0.9;
//    float3 sumColor = float3(0.0f, 0.0f, 0.0f);
//    uint4 firstLayer = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(uv), 0, 0));
//    float3 curPixelColor = unpack_rgb(firstLayer.x).xyz;

//    [unroll(25)]
//    for (int j = 0; j < 25; j++)
//    {
//        float2 aroundUV = uv + offsets[j];
//        float3 RGBtmp = g_MotionBlurColorMap.Sample(g_samLinear, aroundUV).xyz;
//        float4 aroundDepth_SuccessRate_motionVector = g_MotionBlurDepthWeightMap.Sample(g_samLinear, aroundUV);
//        float c_w = abs(1 - dot(RGBtmp, curPixelColor));
//        float d_w = max((1 - abs(unpack_depth(firstLayer.y) - aroundDepth_SuccessRate_motionVector.x) / ZFAR), 0);
//        if (d_w < depthCmp)
//            d_w = 0.3;

//        //float weight = gaussianKernel[j] * aroundDepth_SuccessRate_motionVector.y * c_w;
//        float weight = gaussianKernel[j] * aroundDepth_SuccessRate_motionVector.y * d_w * c_w;
//        sumColor += RGBtmp * weight;
//        sum_w += weight;
//        //if (needDenoging == false && aroundDepth_SuccessRate_motionVector.z != 0.f && aroundDepth_SuccessRate_motionVector.w != 0.f) needDenoging = true;
//    }
//    float3 resColor = sumColor / sum_w;
//    //if (!needDenoging) resColor = unpack_rgb(firstLayer.x);

//    return float4(resColor, 1);
//}

//float4 DenoisingPS(FSQVSOut input) : SV_TARGET
//{
//    float4 color;
////uint4 pos_Sample_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(input.uv), 0, 0));
////color = float4(unpack_rgb(pos_Sample_Color_Depth_MotionVector.x), 0.f);

////color = float4(abs(unpack_MotionVector_XY(pos_Sample_Color_Depth_MotionVector.z)), abs(unpack_MotionVector_Z(pos_Sample_Color_Depth_MotionVector.w)), 0.f);
////color = float4(color.x * color.x, color.y* color.y, color.z * color.z, 0.f);
////color = float4(unpack_depth(pos_Sample_Color_Depth_MotionVector.y) / ZFAR, unpack_depth(pos_Sample_Color_Depth_MotionVector.y) / ZFAR, unpack_depth(pos_Sample_Color_Depth_MotionVector.y) / ZFAR,0);

////color = float4(g_motionVectorMipMap.SampleLevel(g_samPoint, input.uv, 0).xy, 0, 0);
////color = g_firstLayerColorMap.SampleLevel(g_samLinear, input.uv, 0);
////color = g_motionVectorMap.SampleLevel(g_samLinear, input.uv, 0);
////color = g_depthMap.SampleLevel(g_samLinear, input.uv, 0);

////color = float4(denoising(input.uv).y, denoising(input.uv).y, denoising(input.uv).y ,0);

////float depth = g_MotionBlurDepthWeightMap.Sample(g_samLinear, input.uv).y;
////color = float4(depth / ZFAR, depth/ ZFAR, depth / ZFAR, 0);

////float successRate = g_MotionBlurColorMap.Sample(g_samLinear, input.uv).w;
////color = float4(successRate, successRate, successRate, 0);

////color = float4(color.w, color.w, color.w, 1);
////color = float4(input.uv, 0, 1);

    
//    ////////////
////color = g_MotionBlurColorMap.Sample(g_samLinear, input.uv);
    
////color = float4(g_MotionBlurColorMap.Sample(g_samLinear, input.uv).w / 100, g_MotionBlurColorMap.Sample(g_samLinear, input.uv).w / 100, g_MotionBlurColorMap.Sample(g_samLinear, input.uv).w / 100, 1);

//    color = denoising(input.uv);

//    return color;
//}

//technique11 Denoising
//{
//    pass P0
//    {
//        SetRasterizerState(NoCullNoMSAA_RS);
//        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
//        SetPixelShader(CompileShader(ps_5_0, DenoisingPS()));
//    }
//}




#include "function.fx"

Texture2D g_modelTexture;

Texture2DMS<uint4, MSAA_LEVEL> g_color_Depth_motionVectorMap;

Texture2DArray<uint4> g_sorted_Color_Depth_MotionVectorMap;
Texture2DArray<uint4> g_denoised_Color_Depth_MotionVectorMap;
Texture2D g_motionVectorMipMap;
Texture2D g_firstLayerColorMap;
Texture2D g_motionVectorMap;
Texture2D g_depthMap;

Texture2D g_MotionBlurColorMap;
Texture2D g_MotionBlurDepthWeightMap;

//------------------------------------------------------------- stencil buffer clear pass

struct FSQVSOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

FSQVSOut FullScreenQuadVS(uint vertexID : SV_VertexID)
{
    FSQVSOut OutputVS;

    OutputVS.uv = float2((vertexID << 1) & 2, vertexID & 2);
    OutputVS.position = float4(OutputVS.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

    return OutputVS;
}

technique11 StencilBufferClear
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
        SetPixelShader(NULL);
    }
}

//------------------------------------------------------------- texture clear pass

uint4 ClearPS(FSQVSOut Input) : SV_TARGET
{
    return uint4(0, MAX_DEPTH, 2147450879, MAX_DEPTH / 2); //2147450879 == ((16bit)0 + (16bit)0), MAX_DEPTH/2 == (32bit)0
}

technique11 TextureClear
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
        SetPixelShader(CompileShader(ps_5_0, ClearPS()));
        SetRasterizerState(BackCullNoMSAA_RS);
        SetDepthStencilState(NoDepthNoStencil_DS, 0x00000000);
        SetBlendState(NoMRT_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
    }
}

//------------------------------------------------------------- multi Layer Rendering pass (stencil buffer) 

struct RenderingVSInput
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct RenderingVSOut
{
    float4 position : SV_Position;
    float4 prePosition : POSITION1;
    float4 curPosition : POSITION2;
    float2 uv : TEXCOORD0;
};

RenderingVSOut RenderingVS(RenderingVSInput input)
{
    RenderingVSOut output;

    output.position = mul(input.Position, g_worldViewProjection);
    output.prePosition = mul(input.Position, g_previousWorldViewProjection);
    output.curPosition = mul(input.Position, g_worldViewProjection);

    output.uv = input.uv;

    return output;
}

uint4 RenderingPS(RenderingVSOut input) : SV_TARGET
{
    float3 currentPos = homogenious2uv(input.curPosition);
    float3 previousPos = homogenious2uv(input.prePosition);
    float3 motionVector = currentPos - previousPos;
    
    float3 color = float3(g_modelTexture.Sample(g_samPoint, input.uv).xyz);
    
    uint packed_Color = pack_rgb(color);
    uint packed_EyeZ = pack_depth(input.position.w);
    uint packed_MotionVector_XY = pack_MotionVector_XY(motionVector.xy);
    uint packed_MotionVector_Z = pack_MotionVector_Z(motionVector.z);
    
    return uint4(packed_Color, packed_EyeZ, packed_MotionVector_XY, packed_MotionVector_Z);
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

//------------------------------------------------------------- First Layer Rendering pass

struct motionVectorMipMap_colorMap_outPut
{
    float4 MotionVectorMipMap;
    float4 ColorMap;
    float4 MotionVectorMap;
    float4 depthMap;
};

RenderingVSOut motionVectorMipMap_colorMap_renderingVS(RenderingVSInput Input)
{
    RenderingVSOut OutputVS;

    OutputVS.position = mul(Input.Position, g_worldViewProjection);
    OutputVS.prePosition = mul(Input.Position, g_previousWorldViewProjection);
    OutputVS.curPosition = mul(Input.Position, g_worldViewProjection);

    OutputVS.uv = Input.uv;

    return OutputVS;
}

motionVectorMipMap_colorMap_outPut motionVectorMipMap_colorMap_renderingPS(RenderingVSOut input) : SV_TARGET
{
    motionVectorMipMap_colorMap_outPut output;
    
    float3 currentPos = homogenious2uv(input.curPosition);
    float3 previousPos = homogenious2uv(input.prePosition);
    float3 motionVector = currentPos - previousPos;
    
    output.MotionVectorMipMap = float4(motionVector.xy, motionVector.xy * motionVector.xy);
    output.ColorMap = g_modelTexture.Sample(g_samPoint, input.uv);
    output.MotionVectorMap = float4(motionVector, 0);
    output.depthMap = float4(input.position.w, input.position.w, input.position.w, 0);
    
    return output;
}

technique11 motionVectorMipMap_colorMap_Rendering
{
    pass P0
    {
        SetRasterizerState(NoCullNoMSAA_RS);
        SetVertexShader(CompileShader(vs_5_0, motionVectorMipMap_colorMap_renderingVS()));
        SetPixelShader(CompileShader(ps_5_0, motionVectorMipMap_colorMap_renderingPS()));
    }
}

//------------------------------------------------------------- sorting pass

struct FSQVSInput
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float2 uv : TEXCOORD0;
};

FSQVSOut FSQVS(FSQVSInput input)
{
    FSQVSOut OutputVS;

    OutputVS.position = input.Position;
    OutputVS.uv = input.uv;
    return OutputVS;
}

struct sortPSOutPut
{
    uint4 sorted_Layer_0;
    uint4 sorted_Layer_1;
    uint4 sorted_Layer_2;
    uint4 sorted_Layer_3;

    uint4 sorted_depth_0;
    uint4 sorted_depth_1;
    uint4 sorted_depth_2;
    uint4 sorted_depth_3;
    
    //uint4 sorted_Layer_4;
    //uint4 sorted_Layer_5;
    //uint4 sorted_Layer_6;
    //uint4 sorted_Layer_7;

};

sortPSOutPut SortingPS(FSQVSOut input) : SV_TARGET
{
    uint4 frag[8];
    float unpack_Depth[N_LAYER];
    [unroll]
    for (int i = 0; i < 8; ++i)
    {
        frag[i] = g_color_Depth_motionVectorMap.Load(input.position.xy, i);
    }
    BitonicSortF2B(frag, 8);

    float3 color = g_firstLayerColorMap.SampleLevel(g_samLinear, input.uv, 0).xyz;
    float3 motionVector = g_motionVectorMap.SampleLevel(g_samLinear, input.uv, 0).xyz;
    float dpeth = g_depthMap.SampleLevel(g_samLinear, input.uv, 0).x;

    uint packed_Color = pack_rgb(color);
    uint packed_EyeZ = pack_depth(dpeth);
    uint packed_MotionVector_XY = pack_MotionVector_XY(motionVector.xy);
    uint packed_MotionVector_Z = pack_MotionVector_Z(motionVector.z);

    sortPSOutPut output;
    output.sorted_Layer_0 = uint4(packed_Color, packed_EyeZ, packed_MotionVector_XY, packed_MotionVector_Z);
    //output.sorted_Layer_0 = frag[0];
    output.sorted_Layer_1 = frag[1];
    output.sorted_Layer_2 = frag[2];
    output.sorted_Layer_3 = frag[3];

    [unroll]
    for (int j = 0; j < N_LAYER; ++j)
    {
        unpack_Depth[j] = unpack_depth(frag[j].y);
    }

    output.sorted_depth_0 = uint4(pack_depth(unpack_Depth[0]), pack_depth(unpack_Depth[0] * unpack_Depth[0]), 0, 0);
    output.sorted_depth_1 = uint4(pack_depth(unpack_Depth[1]), pack_depth(unpack_Depth[1] * unpack_Depth[1]), 0, 0);
    output.sorted_depth_2 = uint4(pack_depth(unpack_Depth[2]), pack_depth(unpack_Depth[2] * unpack_Depth[2]), 0, 0);
    output.sorted_depth_3 = uint4(pack_depth(unpack_Depth[3]), pack_depth(unpack_Depth[3] * unpack_Depth[3]), 0, 0);
    //output.sorted_Layer_4 = frag[4];
    //output.sorted_Layer_5 = frag[5];
    //output.sorted_Layer_6 = frag[6];
    //output.sorted_Layer_7 = frag[7];
    
    //output.sorted_depth_0 = float4(unpack_depth(frag[0].y), unpack_depth(frag[0].y) * unpack_depth(frag[0].y), 0, 0);
    //output.sorted_depth_1 = float4(unpack_depth(frag[1].y), unpack_depth(frag[1].y) * unpack_depth(frag[1].y), 0, 0);
    //output.sorted_depth_2 = float4(unpack_depth(frag[2].y), unpack_depth(frag[2].y) * unpack_depth(frag[2].y), 0, 0);
    //output.sorted_depth_3 = float4(unpack_depth(frag[3].y), unpack_depth(frag[3].y) * unpack_depth(frag[3].y), 0, 0);

    return output;
}

technique11 Sorting
{
    pass P0
    {
        SetRasterizerState(NoCullNoMSAA_RS);
        SetDepthStencilState(NoDepthNoStencil_DS, 0x00000000);
        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
        SetPixelShader(CompileShader(ps_5_0, SortingPS()));
        SetBlendState(NoMRT_BS, float4(1.0f, 1.0f, 1.0f, 1.0f), 0xffffffff);
    }
}

//------------------------------------------------------------- denoising pass

struct denoisedPSOutPut
{
    uint4 denoised_Layer_0;
    uint4 denoised_Layer_1;
    uint4 denoised_Layer_2;
    uint4 denoised_Layer_3;
};

denoisedPSOutPut DepthCMP(uint4 _currentPixel[4], float2 _uv)
{

    denoisedPSOutPut output;

    float3 averageColor[4] = { float3(0, 0, 0), float3(0, 0, 0), float3(0, 0, 0), float3(0, 0, 0) };
    float averageDepth[4] = { 0, 0, 0, 0 };
    float3 averageMotionVector[4] = { float3(0, 0, 0), float3(0, 0, 0), float3(0, 0, 0), float3(0, 0, 0) };

    static const float depthCmp = 0.95;
    static const int filterSize = 25;

    for (int i = 0; i < filterSize; i++)
    {
        float2 aroundUV = _uv + offsets[i];
        for (int layer = 0; layer < 4; layer++)
        {
            averageColor[layer] += unpack_rgb(g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(aroundUV), layer, 0)).x);
            averageDepth[layer] = unpack_depth(g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(aroundUV), layer, 0)).y);
            averageMotionVector[layer] = float3(unpack_MotionVector_XY(g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(aroundUV), layer, 0)).z), pack_MotionVector_Z(g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(aroundUV), layer, 0)).y));
        }
    }

    for (int j = 0; j < 4; j++)
    {
        averageColor[j] /= filterSize;
        averageDepth[j] /= filterSize;
        averageMotionVector[j] /= filterSize;

        float d = max((1 - abs(unpack_depth(_currentPixel[j].y) - averageDepth[j]) / ZFAR), 0);

        if (d < depthCmp)
            _currentPixel[j] = uint4(pack_rgb(averageColor[j]), 0, 0, 0);
    }

    output.denoised_Layer_0 = _currentPixel[0];
    output.denoised_Layer_1 = _currentPixel[1];
    output.denoised_Layer_2 = _currentPixel[2];
    output.denoised_Layer_3 = _currentPixel[3];

    return output;
}

denoisedPSOutPut SD(uint4 _currentPixel[4], float2 _uv)
{

    denoisedPSOutPut output;

    float3 averageColor[4] = { float3(0, 0, 0), float3(0, 0, 0), float3(0, 0, 0), float3(0, 0, 0) };
    float averageDepth[4] = { 0, 0, 0, 0 };
    float3 averageMotionVector[4] = { float3(0, 0, 0), float3(0, 0, 0), float3(0, 0, 0), float3(0, 0, 0) };
    float meanDepth[4], squaMeanDepth[4], sd[4], Max[4], Min[4];

    static const int constant = 0.5;
    int cnt[4] = { 0, 0, 0, 0 };

    for (int k = 0; k < 4; k++)
    {

        meanDepth[k] = unpack_depth(g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(_uv) / 8, k + 4, 3)).x);
        squaMeanDepth[k] = unpack_depth(g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(_uv) / 8, k + 4, 3)).y);

        sd[k] = sqrt(squaMeanDepth[k] - meanDepth[k] * meanDepth[k]);
        Max[k] = meanDepth[k] + sd[k] * constant;
        Min[k] = meanDepth[k] - sd[k] * constant;
    }

    for (int i = 0; i < 25; i++)
    {
        float2 aroundUV = _uv + offsets[i];
        for (int layer = 0; layer < 4; layer++)
        {
            float depth = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(aroundUV), layer, 0)).y;
            //if (Max[layer] > unpack_depth(depth) && unpack_depth(depth) > Min[layer]) {
            averageColor[layer] += unpack_rgb(g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(aroundUV), layer, 0)).x);
            averageDepth[layer] += depth;
            averageMotionVector[layer] += float3(unpack_MotionVector_XY(g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(aroundUV), layer, 0)).z), pack_MotionVector_Z(g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(aroundUV), layer, 0)).w));
            cnt[layer]++;
            //}
        }
    }

    for (int j = 0; j < 4; j++)
    {
        averageColor[j] /= cnt[j];
        averageDepth[j] /= cnt[j];
        averageMotionVector[j] /= cnt[j];

        //if (Min[j] > unpack_depth(_currentPixel[j].y) && unpack_depth(_currentPixel[j].y) > Max[j]) {
        if (unpack_depth(_currentPixel[j].y) < Min[j] && unpack_depth(_currentPixel[j].y) > Max[j])
        {
            _currentPixel[j] = uint4(pack_rgb(averageColor[j]), 0, 0, 0);
            //_currentPixel[j] = uint4(pack_rgb(float3(1, 0, 0)), 0, 0, 0);
        }
    }
    
    output.denoised_Layer_0 = _currentPixel[0];
    output.denoised_Layer_1 = _currentPixel[1];
    output.denoised_Layer_2 = _currentPixel[2];
    output.denoised_Layer_3 = _currentPixel[3];

    return output;
}

denoisedPSOutPut ReplacePixelByCmpDepth(uint4 _currentPixel[4], float2 _uv)
{

    denoisedPSOutPut output;
    float averageDepth[4] = { 0, 0, 0, 0 };
    float3 averageColor[4] = { float3(0, 0, 0), float3(0, 0, 0), float3(0, 0, 0), float3(0, 0, 0) };
    uint4 currentPixel[4] = _currentPixel;
    static const int constant = 1;
    float depthCmp = 0.97;

    for (int i = 0; i < 25; i++)
    {
        float2 aroundUV = _uv + offsets[i];
        for (int j = 1; j < 4; j++)
        {
            averageDepth[j] += unpack_depth(g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(aroundUV), j, 0)).y);
            averageColor[j] += unpack_rgb(g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(aroundUV), j, 0)).x);
        }
    }
    
    for (int k = 1; k < 4; k++)
    {

        averageDepth[k] /= 25;
        averageColor[k] /= 25;

        float meanDepth = unpack_depth(g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(_uv) / 8, k + 4, 3)).x);
        float squaMeanDepth = unpack_depth(g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(_uv) / 8, k + 4, 3)).y);
        float sd = sqrt(squaMeanDepth - meanDepth * meanDepth);

        float Max = meanDepth + sd * constant;
        float Min = meanDepth - sd * constant;
        
        float d = max((1 - abs(unpack_depth(_currentPixel[k].y) - averageDepth[k]) / ZFAR), 0);

        //if (abs(unpack_depth(_currentPixel[k].y) - averageDepth[k]) > abs(unpack_depth(_currentPixel[k - 1].y) - averageDepth[k])) {
        if (abs(unpack_depth(_currentPixel[k].y) - averageDepth[k]) > abs(unpack_depth(_currentPixel[k - 1].y) - averageDepth[k]) && abs(unpack_depth(_currentPixel[k - 1].y) - averageDepth[k]) < 10)
        {
            currentPixel[k] = uint4(pack_rgb(float3(1, 0, 0)), _currentPixel[k - 1].yzw);
            //currentPixel[k] = _currentPixel[k - 1];
        }
        else if (unpack_depth(_currentPixel[k].y) < Min || unpack_depth(_currentPixel[k].y) > Max)
        {
            currentPixel[k] = uint4(pack_rgb(float3(1, 0, 0)), _currentPixel[k - 1].yzw);
            //currentPixel[k] = uint4(pack_rgb(averageColor[j]), pack_depth(averageDepth[j]), 0, 0);
        }
        else if (d < depthCmp && unpack_depth(_currentPixel[k].y) / ZFAR > 0.1f)
        {
            currentPixel[k] = uint4(pack_rgb(float3(1, 0, 0)), _currentPixel[k - 1].yzw);
            //currentPixel[k] = uint4(pack_rgb(averageColor[j]), pack_depth(averageDepth[j]), 0, 0);
        }
    }

    output.denoised_Layer_0 = currentPixel[0];
    output.denoised_Layer_1 = currentPixel[1];
    output.denoised_Layer_2 = currentPixel[2];
    output.denoised_Layer_3 = currentPixel[3];

    return output;
}

denoisedPSOutPut Denoising_ps(FSQVSOut input) : SV_TARGET
{
    denoisedPSOutPut output;
    uint4 currentPixel[4];

    for (int i = 0; i < 4; i++)
    {
        currentPixel[i] = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(input.uv), i, 0));
    }

    //output = SD(currentPixel, input.uv);
    //output = DepthCMP(currentPixel, input.uv);
    output = ReplacePixelByCmpDepth(currentPixel, input.uv);
    
    return output;
}

technique11 Denoising
{
    pass P0
    {
        SetRasterizerState(NoCullNoMSAA_RS);
        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
        SetPixelShader(CompileShader(ps_5_0, Denoising_ps()));
    }
}

//------------------------------------------------------------- motion blur pass

struct motionBlurPSOutput
{
    float4 color;
    float4 dpeth_successRate_motionVector;
};

float4 searchBoundary(float2 uv)
{
    const int constant = 2;

    float2 mean = g_motionVectorMipMap.SampleLevel(g_samLinear, uv, MIPMAP_LEVEL).xy;
    float2 squaMean = g_motionVectorMipMap.SampleLevel(g_samLinear, uv, MIPMAP_LEVEL).zw;

    float2 standardDeviation = sqrt(squaMean - mean * mean);
    float2 Max = mean + standardDeviation * constant;
    float2 Min = mean - standardDeviation * constant;

    float2 leftTopCorner = max(uv - Max, float2(0.0f, 0.0f));
    float2 rightBottomCorner = min(uv - Min, float2(1.0f, 1.0f));

    return float4(leftTopCorner, rightBottomCorner);
}

motionBlurPSOutput findLineAndGetColor(float2 uv)
{
    static const float epsilon = g_pixelSize.y * EPSILON;
    float2 randomUV = float2(0, 0);
    float sumDepth = 0;

    float2 intersecting_Motion_Vector[N_RANDOMPICK * N_LAYER];
    int nNumintersecting_Motion_Vector = 1;
    uint4 firstLayer = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(uv), 0, 0));
    intersecting_Motion_Vector[0] = unpack_MotionVector_XY(firstLayer.z);

    float4 motionBoundary = searchBoundary(uv);
    motionBlurPSOutput output;

    for (int sampleCnt = 0; sampleCnt < N_RANDOMPICK; sampleCnt++)
    {

        randomUV = float2(Random(float2(uv.x + sampleCnt * 0.0002468f, uv.y + sampleCnt * 0.0003456f)), Random(float2(uv.x + sampleCnt * 0.0001357f, uv.y + sampleCnt * 0.0004567f)));
        float2 currentCandidate = float2(motionBoundary.x + randomUV.x * abs(motionBoundary.z - motionBoundary.x), motionBoundary.y + randomUV.y * abs(motionBoundary.w - motionBoundary.y));

        for (int Layer = 0; Layer < N_LAYER; Layer++)
        {
            uint4 packed_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(currentCandidate), Layer, 0));
            float2 candidate_MotionVector = unpack_MotionVector_XY(packed_Color_Depth_MotionVector.z);

            bool isFound = false;
            for (int j = 0; j < N_SEARCH; j++)
            {

                if (intersect(uv, candidate_MotionVector, currentCandidate, epsilon))
                {
                    isFound = true;
                    break;
                }

                currentCandidate = uv - candidate_MotionVector * sampleCnt / N_RANDOMPICK;

                uint4 packed_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(currentCandidate), Layer, 0));
                candidate_MotionVector = unpack_MotionVector_XY(packed_Color_Depth_MotionVector.z);
            }

            if (isFound)
            {
                int k;
                for (k = 0; k < nNumintersecting_Motion_Vector; k++)
                {
                    if (distance(candidate_MotionVector, intersecting_Motion_Vector[k]) < g_pixelSize.y)
                        break;
                }

                if (k == nNumintersecting_Motion_Vector)
                {
                    intersecting_Motion_Vector[k] = candidate_MotionVector;
                    nNumintersecting_Motion_Vector++;
                }
            }
        }
    }

    if (nNumintersecting_Motion_Vector == 1 && -g_pixelSize.x < intersecting_Motion_Vector[0].x && intersecting_Motion_Vector[0].x < g_pixelSize.x && -g_pixelSize.y < intersecting_Motion_Vector[0].y && intersecting_Motion_Vector[0].y < g_pixelSize.y)
    {
        output.color = float4(unpack_rgb(firstLayer.x), 0);
        output.dpeth_successRate_motionVector = float4(unpack_depth(firstLayer.y), 1, 0, 0);
    }
    else
    {
        bool searchFirstLayer = false;
        float nNumOfSucess = 0;
        float3 colorSum = float3(0.0f, 0.0f, 0.0f);
        for (int time = 0; time < N_SAMPLINGTIME; time++)
        {
            randomUV = float2(Random(float2(uv.x + time * 0.0002468f, uv.y + time * 0.0003456f)), Random(float2(uv.x + time * 0.0001357f, uv.y + time * 0.0004567f)));
            float targetDepth = ZFAR;
            float3 targetColor = float3(-1.f, 0.f, 0.f);
            for (int j = 0; j < nNumintersecting_Motion_Vector; j++)
            {
                for (int layer = 0; layer < N_LAYER; layer++)
                {
                    float2 inverseMotionVector = uv - intersecting_Motion_Vector[j];

                    float clipped_ratio = MOTIONVECTOR_MAX;
                    float ratio = 1.f;
                     
                    if (inverseMotionVector.x < 0.f)
                    {
                        if (clipped_ratio > uv.x)
                            clipped_ratio = abs(uv.x / intersecting_Motion_Vector[j].x);
                    }
                    else if (inverseMotionVector.x > 1.f)
                    {
                        if (clipped_ratio > 1.f - uv.x)
                            clipped_ratio = (1.f - uv.x) / abs(intersecting_Motion_Vector[j].x);
                    }

                    if (inverseMotionVector.y < 0.f)
                    {
                        if (clipped_ratio > uv.y)
                            clipped_ratio = abs(uv.y / intersecting_Motion_Vector[j].y);
                    }
                    else if (inverseMotionVector.y > 1.f)
                    {
                        if (clipped_ratio > 1.f - uv.y)
                            clipped_ratio = (1.f - uv.y) / abs(intersecting_Motion_Vector[j].y);
                    }

                    if (clipped_ratio != MOTIONVECTOR_MAX)
                        ratio = clipped_ratio;

                    float2 Motion_Vector_Sample_Pos = uv - intersecting_Motion_Vector[j] * ratio * (time + randomUV) / N_SAMPLINGTIME;


                    uint4 pos_Sample_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(Motion_Vector_Sample_Pos), layer, 0));
                    float3 pos_Sample_MotionVector = float3(unpack_MotionVector_XY(pos_Sample_Color_Depth_MotionVector.z), unpack_MotionVector_Z(pos_Sample_Color_Depth_MotionVector.w));

                    if (distance(intersecting_Motion_Vector[j], pos_Sample_MotionVector.xy) < g_pixelSize.y)
                    {
                        pos_Sample_MotionVector.z *= (float) (time + randomUV) / N_SAMPLINGTIME;
                        float pos_Sample_Depth = unpack_depth(pos_Sample_Color_Depth_MotionVector.y);

                        if (pos_Sample_MotionVector.z + pos_Sample_Depth < targetDepth)
                        {
                            if (layer == 0)
                                searchFirstLayer = true;
                            targetColor = unpack_rgb(pos_Sample_Color_Depth_MotionVector.x);
                            targetDepth = pos_Sample_MotionVector.z + pos_Sample_Depth;
                            break;
                        }
                    }
                }
            }

            if (targetColor.x >= 0.f)
            {
                colorSum += targetColor;
                sumDepth += targetDepth;
                nNumOfSucess++;
            }
        }

        float successRate = 0;
        float3 resColor = unpack_rgb(firstLayer.x);

        if (nNumOfSucess > 0)
        {
            resColor = colorSum / nNumOfSucess;
            successRate = nNumOfSucess / N_SAMPLINGTIME;
        }
        if (!searchFirstLayer)
        {
            successRate = 0;
        }
        output.color = float4(resColor, 1);
        output.dpeth_successRate_motionVector = float4(sumDepth / nNumOfSucess, successRate * successRate, unpack_MotionVector_XY(firstLayer.z));
    }

    return output;
}

motionBlurPSOutput MakeMotionBlurPS(FSQVSOut input) : SV_TARGET
{
    return findLineAndGetColor(input.uv);
}

technique11 MakeMotionBlur
{
    pass P0
    {
        SetRasterizerState(NoCullNoMSAA_RS);
        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
        SetPixelShader(CompileShader(ps_5_0, MakeMotionBlurPS()));
    }
}

//------------------------------------------------------------- Denoising based on pixel reliability pass

float4 denoising(float2 uv)
{
    float sum_w = 0, depthCmp = 0.99;
    float3 sumColor = float3(0.0f, 0.0f, 0.0f);
    uint4 firstLayer = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(uv), 0, 0));
    float3 curPixelColor = unpack_rgb(firstLayer.x).xyz;

    [unroll(25)]
    for (int j = 0; j < 25; j++)
    {
        float2 aroundUV = uv + offsets[j];
        float3 RGBtmp = g_MotionBlurColorMap.Sample(g_samLinear, aroundUV).xyz;
        float4 aroundDepth_SuccessRate_motionVector = g_MotionBlurDepthWeightMap.Sample(g_samLinear, aroundUV);
        float c_w = abs(1 - dot(RGBtmp, curPixelColor));
        float d_w = max((1 - abs(unpack_depth(firstLayer.y) - aroundDepth_SuccessRate_motionVector.x) / ZFAR), 0);
        if (d_w < depthCmp)
            d_w = 0.3;

        //float weight = gaussianKernel[j] * aroundDepth_SuccessRate_motionVector.y * c_w;
        float weight = gaussianKernel[j] * aroundDepth_SuccessRate_motionVector.y * d_w * c_w;
        sumColor += RGBtmp * weight;
        sum_w += weight;
        //if (needDenoging == false && aroundDepth_SuccessRate_motionVector.z != 0.f && aroundDepth_SuccessRate_motionVector.w != 0.f) needDenoging = true;
    }
    float3 resColor = sumColor / sum_w;

    return float4(resColor, 1);
}

float4 DenoisingPS(FSQVSOut input) : SV_TARGET
{
    float4 color;
    //uint4 pos_Sample_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(input.uv), 2, 0));
    //uint4 pos_Sample_Color_Depth_MotionVector = g_denoised_Color_Depth_MotionVectorMap.Load(int4(GetScreenUV(input.uv), 1, 0));
    //float4 pos_Sample_Color_Depth_MotionVector = g_sorted_Color_Depth_MotionVectorMap.Load(int4(input.position.xy , 0, 0));
    //color = float4(unpack_rgb(pos_Sample_Color_Depth_MotionVector.x), 0.f);
    //color = float4(unpack_depth(pos_Sample_Color_Depth_MotionVector.x),0,0, 0.f);

    //color = float4(abs(unpack_MotionVector_XY(pos_Sample_Color_Depth_MotionVector.z)), abs(unpack_MotionVector_Z(pos_Sample_Color_Depth_MotionVector.w)), 0.f);
    //color = float4(color.x * color.x, color.y* color.y, color.z * color.z, 0.f);
    //color = float4(unpack_depth(pos_Sample_Color_Depth_MotionVector.y / ZFAR), unpack_depth(pos_Sample_Color_Depth_MotionVector.y / ZFAR), unpack_depth(pos_Sample_Color_Depth_MotionVector.y / ZFAR), 0);
    //color = float4(unpack_depth(pos_Sample_Color_Depth_MotionVector.x) / ZFAR, unpack_depth(pos_Sample_Color_Depth_MotionVector.x) / ZFAR, unpack_depth(pos_Sample_Color_Depth_MotionVector.x) / ZFAR,0);

    //color = float4(g_motionVectorMipMap.SampleLevel(g_samPoint, input.uv, 0).xy, 0, 0);
    //color = g_firstLayerColorMap.SampleLevel(g_samLinear, input.uv, 0);
    //color = g_motionVectorMap.SampleLevel(g_samLinear, input.uv, 0);
    //color = g_depthMap.SampleLevel(g_samLinear, input.uv, 0);

    //color = g_MotionBlurColorMap.Sample(g_samLinear, input.uv);
    //color = float4(g_MotionBlurColorMap.Sample(g_samLinear, input.uv).w / 100, g_MotionBlurColorMap.Sample(g_samLinear, input.uv).w / 100, g_MotionBlurColorMap.Sample(g_samLinear, input.uv).w / 100, 1);

    color = denoising(input.uv);

    //color = float4(denoising(input.uv).y, denoising(input.uv).y, denoising(input.uv).y ,0);

    //float depth = g_MotionBlurDepthWeightMap.Sample(g_samLinear, input.uv).y;
    //color = float4(depth / ZFAR, depth/ ZFAR, depth / ZFAR, 0);

    //float successRate = g_MotionBlurColorMap.Sample(g_samLinear, input.uv).w;
    //color = float4(successRate, successRate, successRate, 0);

    //color = float4(color.w, color.w, color.w, 1);
    //color = float4(input.uv, 0, 1);

    //if (input.uv.x > 0.04f && input.uv.x < 0.05f)  color = float4(1, 0, 0, 0);
    
    //color = float4(1.f, 0.f, 0.f, 0.f);
    

    return color;
}

technique11 DenoisingWithReliability
{
    pass P0
    {
        SetRasterizerState(NoCullNoMSAA_RS);
        SetVertexShader(CompileShader(vs_5_0, FullScreenQuadVS()));
        SetPixelShader(CompileShader(ps_5_0, DenoisingPS()));
    }
}