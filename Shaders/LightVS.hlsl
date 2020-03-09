cbuffer cbWorld : register(b0)
{
	matrix World;
}

cbuffer cbViewProjection : register(b1)
{
	matrix ViewProjection;
}

cbuffer LightAttribute : register(b2)
{
	float4 LightPosition;
}

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 TextureCoord : TEXCOORD;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 LightDirection : LIGHT_DIRECTION;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(input.Position, World);

	output.Normal = normalize(mul(input.Normal, (float3x3) World).xyz);
	output.LightDirection = normalize((float3) LightPosition - output.Position.xyz);

	output.Position = mul(output.Position, ViewProjection);

	output.TextureCoord = input.TextureCoord;

	return output;
}