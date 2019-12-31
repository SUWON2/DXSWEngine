struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
};

float4 PS(PS_INPUT input) : SV_Target
{
	return input.Color;
}