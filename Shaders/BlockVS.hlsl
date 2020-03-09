cbuffer cbWorld : register(b0)
{
    matrix World;
}

cbuffer cbViewProjection : register(b1)
{
    matrix ViewProjection;
}

struct VS_INPUT
{
    float4 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    
    row_major float4x4 InstanceWorld : I_WORLD;
    int InstanceTextureIndex : I_TEXTURE_INDEX;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
    int TextureIndex : TEXTURE_INDEX;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(input.Position, input.InstanceWorld);
    output.Position = mul(output.Position, ViewProjection);
	
    output.TextureCoord = input.TextureCoord;
    output.TextureIndex = input.InstanceTextureIndex;
    
    return output;
}