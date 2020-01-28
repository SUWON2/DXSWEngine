struct PS_INPUT
{
	float4 Position : SV_POSITION;
    float3 Color : COLOR0;
};

float4 PS(PS_INPUT input) : SV_Target
{
    return float4(input.Color.xyz, 1.0f);
}