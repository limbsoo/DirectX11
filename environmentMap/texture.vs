////////////////////////////////////////////////////////////////////////////////
// Filename: texture.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer PerFrameBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output = (PixelInputType)0;
    float4 position = float4(input.position,1.f); 


	// 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ​​계산합니다.
    output.position = mul(position, worldMatrix);




    // output.position = mul(output.position, viewMatrix);
    //output.position = mul(output.position, projectionMatrix);
    
    //output.postion.x = float4(input.position, 1.f);

	// 픽셀 쉐이더의 텍스처 좌표를 설정합니다
	output.tex = input.tex;
    
    return output;
}