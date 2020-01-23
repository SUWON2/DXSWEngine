Texture2D GeneralTexture : register(t0);
SamplerState SamplerLinear : register(s0);

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 LightDirection : LIGHT_DIRECTION;
};

float4 PS(PS_INPUT input) : SV_Target
{
	const float brightness = saturate(dot(input.Normal, input.LightDirection));
	const float3 rgbColor = float3(1.0f * brightness, 0.0f, 0.0f);

    return GeneralTexture.Sample(SamplerLinear, input.TextureCoord) * float4(rgbColor, 1.0f) + float4(0.1f, 0.1f, 0.1f, 0.1f);
}