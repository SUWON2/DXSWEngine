struct PS_INPUT
{
	float4 Position : SV_POSITION;
    float4 DepthPosition : DEPTH_POSITION;
};

float4 PS(PS_INPUT input) : SV_Target
{
    const float depth = input.DepthPosition.z / input.DepthPosition.w;
	
    return float4(depth, depth, depth, 1.0f);
}