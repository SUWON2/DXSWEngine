cbuffer cbViewProjection : register(b0)
{
    matrix ViewProjection;
}

struct VS_INPUT
{
    float4 position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    
    row_major float4x4 InstanceWorld : I_WORLD;
};

float4 VS(VS_INPUT input) : SV_Position
{
    input.position = mul(input.position, input.InstanceWorld);
    input.position = mul(input.position, ViewProjection);
	
    return input.position;
}