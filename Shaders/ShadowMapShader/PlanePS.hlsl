Texture2D ShadowMapTexture : register(t0);
Texture2D GeneralTexture : register(t1);
SamplerState SamplerLinear : register(s0);
SamplerState SamplerLinearShadow : register(s1);

cbuffer cbColor : register(b1)
{
    float3 Color;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 LightViewPosition : LIGHT_VIEW_POSITION;
    float2 TextureCoord : TEXCOORD;
};

float4 PS(PS_INPUT input) : SV_Target
{
    float2 shadowMapTextureCoord;
    shadowMapTextureCoord.x = input.LightViewPosition.x * 0.5f + 0.5f;
    shadowMapTextureCoord.y = -input.LightViewPosition.y * 0.5f + 0.5f;
    
    float3 brightness = (float3) 1.0f;
    //float3 brightness = (float3) 1.0f;
    
    //if (saturate(shadowMapTextureCoord.x) == shadowMapTextureCoord.x 
    //    && saturate(shadowMapTextureCoord.y) == shadowMapTextureCoord.y)
    //{
    //    const float shadowMapDepth = ShadowMapTexture.Sample(SamplerLinearShadow, shadowMapTextureCoord).r;
    
    //    // ºû °üÁ¡¿¡¼­ ¹Ù¶óºÃÀ» ¶§ÀÇ ±íÀÌ
    //    const float lightDepth = input.LightViewPosition.z;
    
    //    if (lightDepth < shadowMapDepth)
    //    {
    //        brightness = 1.0f;
    //    }
    //}
    //else
    //{
    //    brightness = 1.0f;
    //}
    
    return GeneralTexture.Sample(SamplerLinear, input.TextureCoord) * float4(saturate(Color * brightness), 1.0f);
}