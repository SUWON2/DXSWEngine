Texture2D ShadowMapTexture : register(t0);
Texture2D GeneralTexture : register(t1);
SamplerState SamplerLinear : register(s0);
SamplerState SamplerLinearShadow : register(s1);

cbuffer cbColor : register(b0)
{
	float3 Color;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 LightViewPosition : LIGHT_VIEW_POSITION; // 라이트 관점에서의 위치를 말하는 거임, 변수명 바꿀 필요 있음
    float2 TextureCoord : TEXCOORD;
    float3 Diffuse : DIFFUSE;
};

float4 PS(PS_INPUT input) : SV_Target
{
    float2 shadowMapTextureCoord;
    shadowMapTextureCoord.x = input.LightViewPosition.x / input.LightViewPosition.w * 0.5f + 0.5f;
    shadowMapTextureCoord.y = -input.LightViewPosition.y / input.LightViewPosition.w * 0.5f + 0.5f;
    
    float3 brightness = (float3) 1.0f;
    //float3 brightness = (float3) 0.3f;
    
    //if (saturate(shadowMapTextureCoord.x) == shadowMapTextureCoord.x 
    //    && saturate(shadowMapTextureCoord.y) == shadowMapTextureCoord.y)
    //{
    //    const float shadowMapDepth = ShadowMapTexture.Sample(SamplerLinearShadow, shadowMapTextureCoord).r;
    
    //    // 빛 관점에서 바라봤을 때의 깊이
    //    //const float lightDepth = input.LightViewPosition.z - 0.0311f;
    //    const float lightDepth = input.LightViewPosition.z;
    
    //    if (lightDepth < shadowMapDepth)
    //    {
    //        //brightness += saturate(input.Diffuse) * Color;
    //        brightness = 1.0f;
    //    }
    //}
    //else
    //{
    //    //brightness += saturate(input.Diffuse) * Color;
    //    brightness = 1.0f;
    //}
	
    return GeneralTexture.Sample(SamplerLinear, input.TextureCoord) * float4(brightness, 1.0f) * float4(Color, 1.0f);
}