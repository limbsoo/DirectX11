//////////////////////////////////////////////////////////////////////////////////
//// Filename: shadow.vs
//////////////////////////////////////////////////////////////////////////////////
//
//
///////////////
//// GLOBALS //
///////////////
//cbuffer MatrixBuffer
//{
//    matrix worldMatrix;
//    matrix viewMatrix;
//    matrix projectionMatrix;
//    matrix lightViewMatrix;
//    matrix lightProjectionMatrix;
//};
//
////////////////////////
//// CONSTANT BUFFERS //
////////////////////////
//cbuffer LightBuffer
//{
//    float3 lightPosition;
//    float padding;
//};
//
////////////////
//// TYPEDEFS //
////////////////
//struct VertexInputType
//{
//    float4 position : POSITION;
//    float2 texure : TEXCOORD;
//    float3 normal : NORMAL;
//    float4 cameraPosition : BLENDWEIGHT;
//};
//
//struct PixelInputType
//{
//    float4 position : SV_POSITION;
//    float2 texure : TEXCOORD0;
//    float3 normal : NORMAL;
//    //float4 lightViewPosition : TEXCOORD1;
//    //float3 lightDir : TEXCOORD2;
//};
//
//
//////////////////////////////////////////////////////////////////////////////////
//// Vertex Shader
//////////////////////////////////////////////////////////////////////////////////
//PixelInputType EnvironmentVertexShader(VertexInputType input)
//{
//    PixelInputType output;
//
//    float cos;
//    float4 s;
//
//    output.texure = input.texure;
//
//    output.normal = mul(input.normal, (float3x3)worldMatrix);
//
//    output.position = mul(input.position, worldMatrix);
//
//    output.position = input.cameraPosition - output.position;
//
//    output.position = normalize(output.position);
//
//    cos = output.normal * output.position;
//
//    s.x = output.normal.x * cos;
//    s.y = output.normal.y * cos;
//    s.z = output.normal.z * cos;
//
//    s.x = (s.x * 2) - output.position.x;
//    s.y = (s.y * 2) - output.position.y;
//    s.z = (s.z * 2) - output.position.z;
//
//    output.normal.x = s.x;
//    output.normal.y = s.y;
//    output.normal.z = s.z;
//
//    return output;
//}


////////////////////////////////////////////////////////////////////////////////
// Filename: shadow.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer
{
    float3 lightPosition;
    float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 texure : TEXCOORD;
    float3 normal : NORMAL;
    float4 camera : BLENDWEIGHT;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 texure : TEXCOORD0;
    float3 normal : TEXCOORD3;
    float4 lightViewPosition : TEXCOORD1;
    float3 lightDir : TEXCOORD2;
    float3 rVector : TEXCOORD4;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ShadowVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

    ///////////////
    //makeRVector//
    ///////////////hjkhjkjkhjkh
    float cos;
    float4 s;
    float3 cos1;

    //Vector4 normal(m_worldVertexNormal[i], 1);
    output.normal = mul(input.normal, (float3x3)worldMatrix);

    //Vector4 postition(m_worldVertex[i], 1);
    output.position = mul(input.position, worldMatrix);

    //postition = eye - postition;
    output.position = input.camera - output.position;

    //postition = normalize(postition);
    output.position = normalize(output.position);

    //cos = normal * postition;
    //cos = output.normal * output.position;

    // 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;

    // 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ??계산합니다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // 광원에 의해 보았을 때 vertice의 위치를 ??계산합니다.
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

    // 픽셀 쉐이더의 텍스처 좌표를 저장한다.
    output.texure = input.texure;

    // 월드 행렬에 대해서만 법선 벡터를 계산합니다.
    output.normal = mul(input.normal, (float3x3)worldMatrix);

    // 법선 벡터를 정규화합니다.
    output.normal = normalize(output.normal);

    // 세계의 정점 위치를 계산합니다.
    worldPosition = mul(input.position, worldMatrix);

    // 빛의 위치와 세계의 정점 위치를 기반으로 빛의 위치를 ??결정합니다.
    //makeWorldLightingDepth
    output.lightDir = lightPosition.xyz - worldPosition.xyz;

    // 라이트 위치 벡터를 정규화합니다.
    output.lightDir = normalize(output.lightDir);












    cos1.x = output.normal.x * worldPosition.x;
    cos1.y = output.normal.y * worldPosition.y;
    cos1.z = output.normal.z * worldPosition.z;

    cos = cos1.x + cos1.y + cos1.z;

    s.x = output.normal.x * cos;
    s.y = output.normal.y * cos;
    s.z = output.normal.z * cos;

    s = (s * 2) - worldPosition;

    output.rVector.x = s.x;
    output.rVector.y = s.y;
    output.rVector.z = s.z;






    return output;
}

