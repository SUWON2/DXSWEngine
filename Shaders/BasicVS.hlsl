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
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, ViewProjection);

    return output;
}