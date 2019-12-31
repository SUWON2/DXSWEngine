cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
};

VS_OUTPUT VS(float4 Position : POSITION, float4 Color : COLOR)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
	output.Color = Color;

	return output;
}