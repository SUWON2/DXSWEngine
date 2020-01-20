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
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, ViewProjection);
    output.TextureCoord = input.TextureCoord;

    return output;
}