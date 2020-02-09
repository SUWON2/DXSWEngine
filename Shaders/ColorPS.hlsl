cbuffer cbColor : register(b0)
{
	float4 Color;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
};

float4 PS(PS_INPUT input) : SV_Target
{
	return Color;
}