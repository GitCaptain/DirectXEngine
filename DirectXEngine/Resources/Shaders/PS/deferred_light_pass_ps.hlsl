#include "calculate_phong_tex.hlsli"

Texture2D positionTex:        register(t0);
Texture2D colorTex:           register(t1);
Texture2D normalTex:          register(t2);
Texture1D lightTexture:       register(t3);

float4 main(float4 screenPos: SV_POSITION) : SV_TARGET{
    float3 texCoord = float3(screenPos.xy, 0);
    float3 sampleColor = colorTex.Load(texCoord);
    float3 normal = normalTex.Load(texCoord);
    float3 worldPosition = positionTex.Load(texCoord);

    return calculateLightTex(worldPosition, sampleColor, normal, lightTexture);
}
