#include "calculate_phong.hlsli"

Texture2D ModelTex:           register(t0);
Texture2D positionTex:        register(t1);
Texture2D colorTex:           register(t2);
Texture2D normalTex:          register(t3);
SamplerState perPixelSampler: register(s0);


float4 main(float4 screenPos: SV_POSITION) : SV_TARGET{
    float3 texCoord = float3(screenPos.xy, 0);
    float3 sampleColor = colorTex.Load(texCoord);
    float3 normal = normalTex.Load(texCoord);
    float3 worldPosition = positionTex.Load(texCoord);

    return calculateLight(worldPosition, sampleColor, normal);
}
