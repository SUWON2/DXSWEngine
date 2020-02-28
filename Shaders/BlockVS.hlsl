cbuffer cbWorld : register(b0)
{
	matrix World;
}

cbuffer cbViewProjection : register(b1)
{
	matrix ViewProjection;
}

cbuffer cbTextureCoordScale : register(b2)
{
	float2 TextureCoordScale;
}

cbuffer cbLightPosition : register(b3)
{
	float3 WorldLightPosition;
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
	float3 Diffuse : DIFFUSE;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(input.Position, World);

	const float3 worldLightDirection = normalize(WorldLightPosition - output.Position);
	output.Diffuse = saturate(dot(normalize(input.Normal), worldLightDirection));

	output.Position = mul(output.Position, ViewProjection);

	output.TextureCoord = input.TextureCoord * TextureCoordScale;

	return output;
}