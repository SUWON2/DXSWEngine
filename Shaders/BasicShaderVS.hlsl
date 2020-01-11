cbuffer cbWorld : register(b0)
{
    matrix World;
}

cbuffer cbViewProjection : register(b1)
{
    matrix ViewProjection;
}


struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
};

VS_OUTPUT VS(float4 Position : POSITION, float3 Normal : NORMAL)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(Position, World);
    output.Position = mul(output.Position, ViewProjection);

    return output;
}