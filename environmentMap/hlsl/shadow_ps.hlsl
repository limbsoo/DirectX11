////////////////////////////////////////////////////////////////////////////////
// Filename: shadow.ps
////////////////////////////////////////////////////////////////////////////////



//////////////
// TEXTURES //
//////////////

Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D sphericalTexture : register(t2);

//�޸𸮿� ���̴� ���ҽ��� �ְڴ�


//���̴� �� 5(SM5.0) ���ҽ� ������ register Ű���带 ����Ͽ� ���ҽ��� ���� �߿��� ������ HLSL �����Ϸ��� �������մϴ�. 
//t ? SRV(���̴� ���ҽ� ��)


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);

//s ? ���÷�


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


	//�� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� �ؽ�ó���� �ȼ� ������ ���ø��մϴ�.
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


	//// �ε� �Ҽ��� ���е� ������ �ذ��� ���̾ ���� �����մϴ�.
	bias = 0.001f;

	//��� �ȼ��� ���� �⺻ ��� ������ �ֺ� ���� ������ �����մϴ�.
    color = ambientColor;

	//���� �� �ؽ�ó ��ǥ�� ����մϴ�.
	// tu�� tv��ǥ�� -1�� 1������ ���� ������ �Ǳ� ������ 2�� ���ϰ� 0.5�� ���Ͽ� 0�� 1������ ������ �����ݴϴ�.

	projectedTexureCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectedTexureCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	 //���� �� ��ǥ�� 0���� 1 ������ �ִ��� �����մϴ�. �׷��ٸ��� �ȼ��� ���� ������ �ֽ��ϴ�.
	if((saturate(projectedTexureCoord.x) == projectedTexureCoord.x) && (saturate(projectedTexureCoord.y) == projectedTexureCoord.y))
	{

		//float scalaN;
		//scalaN = sqrt(pow(input.rVector.x, 2) + pow(input.rVector.y, 2) + pow(input.rVector.z + 1, 2));

		//projectedTexureCoord.x = projectedTexureCoord.x / scalaN + 1;
		//projectedTexureCoord.y = projectedTexureCoord.y / scalaN + 1;


		// ���� �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� ���� �ؽ�ó���� ������ �� ���� ���� ���ø��մϴ�.
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectedTexureCoord).r;

		// ���� ���̸� ����մϴ�.
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// lightDepthValue���� ���̾�� ���ϴ�.
		lightDepthValue = lightDepthValue - bias;

		// ������ �� ���� ���̿� ���� ���̸� ���Ͽ��� �ȼ��� ���� ó������ �������� �����մϴ�.
		// ���� ��ü �տ� ������ �ȼ��� ���߰�, �׷��� ������ ��ü (��Ŭ ���)�� �׸��ڸ� �帮 ��� �������� �ȼ��� �׸��ڷ� �׸��ϴ�.
		if(lightDepthValue < depthValue)
		{
		    // �� �ȼ��� ���� ���� ����մϴ�.
			lightIntensity = saturate(dot(input.normal, input.lightDir));

		    if(lightIntensity > 0.0f)
			{
				// Ȯ�� ���� �� ������ �翡 ���� ���� Ȯ�� ���� �����մϴ�.

				//diifuse specular +
				//color += (diffuseColor * lightIntensity) + sphereColor + textureColor - 1.0f;

				color += (diffuseColor * lightIntensity* textureColor)+ sphereColor - 0.2f;

				// ���� ���� ������ ä��ϴ�.
				color = saturate(color);
			}
		}
	}




    return sphereColor;
}



