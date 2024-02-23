////////////////////////////////////////////////////////////////////////////////
// Filename: shadow.ps
////////////////////////////////////////////////////////////////////////////////



//////////////
// TEXTURES //
//////////////

Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D sphericalTexture : register(t2);

//메모리에 쉐이더 리소스를 주겠다


//셰이더 모델 5(SM5.0) 리소스 구문은 register 키워드를 사용하여 리소스에 대한 중요한 정보를 HLSL 컴파일러에 릴레이합니다. 
//t ? SRV(셰이더 리소스 뷰)


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);

//s ? 샘플러


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 texure : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightDir : TEXCOORD2;

	float3 rVector : NORMAL1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ShadowPixelShader(PixelInputType input) : SV_TARGET
{
	float bias;
	float4 sphereColor, color;
	float2 projectedTexureCoord;
	float depthValue;
	float lightDepthValue;
    float lightIntensity;
	float4 textureColor;

	float4 sphericalTextureColor;


	//이 텍스처 좌표 위치에서 샘플러를 사용하여 텍스처에서 픽셀 색상을 샘플링합니다.
	textureColor = shaderTexture.Sample(SampleTypeWrap, input.texure);

	input.rVector.y *= -1;

	float scalaN;
	scalaN = sqrt(pow(input.rVector.x, 2) + pow(input.rVector.y, 2) + pow(input.rVector.z + 1, 2));

	sphericalTextureColor.x = (input.rVector.x / scalaN + 1) / 2;
	sphericalTextureColor.y = (input.rVector.y / scalaN + 1) / 2;
	sphereColor = sphericalTexture.Sample(SampleTypeWrap, sphericalTextureColor);

	//return float4(input.rVector.xy, 0.f,1);

	//color = sphereColor  *
	//color = sphereColor + textureColor;


	//// 부동 소수점 정밀도 문제를 해결할 바이어스 값을 설정합니다.
	bias = 0.001f;

	//모든 픽셀에 대해 기본 출력 색상을 주변 광원 값으로 설정합니다.
    color = ambientColor;

	//투영 된 텍스처 좌표를 계산합니다.
	// tu와 tv좌표가 -1과 1사이의 값을 가지게 되기 때문에 2를 더하고 0.5를 곱하여 0과 1사이의 범위로 맞춰줍니다.

	projectedTexureCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectedTexureCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	 //투영 된 좌표가 0에서 1 범위에 있는지 결정합니다. 그렇다면이 픽셀은 빛의 관점에 있습니다.
	if((saturate(projectedTexureCoord.x) == projectedTexureCoord.x) && (saturate(projectedTexureCoord.y) == projectedTexureCoord.y))
	{

		//float scalaN;
		//scalaN = sqrt(pow(input.rVector.x, 2) + pow(input.rVector.y, 2) + pow(input.rVector.z + 1, 2));

		//projectedTexureCoord.x = projectedTexureCoord.x / scalaN + 1;
		//projectedTexureCoord.y = projectedTexureCoord.y / scalaN + 1;


		// 투영 된 텍스처 좌표 위치에서 샘플러를 사용하여 깊이 텍스처에서 섀도우 맵 깊이 값을 샘플링합니다.
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectedTexureCoord).r;

		// 빛의 깊이를 계산합니다.
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// lightDepthValue에서 바이어스를 뺍니다.
		lightDepthValue = lightDepthValue - bias;

		// 섀도우 맵 값의 깊이와 빛의 깊이를 비교하여이 픽셀을 음영 처리할지 조명할지 결정합니다.
		// 빛이 객체 앞에 있으면 픽셀을 비추고, 그렇지 않으면 객체 (오클 루더)가 그림자를 드리 우기 때문에이 픽셀을 그림자로 그립니다.
		if(lightDepthValue < depthValue)
		{
		    // 이 픽셀의 빛의 양을 계산합니다.
			lightIntensity = saturate(dot(input.normal, input.lightDir));

		    if(lightIntensity > 0.0f)
			{
				// 확산 색과 광 강도의 양에 따라 최종 확산 색을 결정합니다.

				//diifuse specular +
				//color += (diffuseColor * lightIntensity) + sphereColor + textureColor - 1.0f;

				color += (diffuseColor * lightIntensity* textureColor)+ sphereColor - 0.2f;

				// 최종 빛의 색상을 채웁니다.
				color = saturate(color);
			}
		}
	}




    return sphereColor;
}



