cbuffer cbWorld : register(b0)
{
    matrix World;
}

cbuffer cbViewProjection : register(b1)
{
    matrix ViewProjection;
}

cbuffer cbColor : register(b2)
{
    float3 Color;
}

struct VS_INPUT
{
    float4 Position : POSITION;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, ViewProjection);
    output.Color = Color;
    
    return output;
}