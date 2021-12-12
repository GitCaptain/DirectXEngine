#include "calculate_phong_tex.hlsli"
#include "gamma_correction.hlsli"
#include "tone_mapping.hlsli"

Texture2D positionTex:        register(t0);
Texture2D colorTex:           register(t1);
Texture2D normalTex:          register(t2);
Texture2D specularTex:        register(t3);
Texture1D lightTexture:       register(t4);

float4 main(float4 screenPos: SV_POSITION) : SV_TARGET{
    float3 texCoord = float3(screenPos.xy, 0);
    float3 sampleColor = colorTex.Load(texCoord);
    float3 normal = normalTex.Load(texCoord);
    float3 worldPosition = positionTex.Load(texCoord);
    float4 specular = specularTex.Load(texCoord);

    float4 lightedColor = calculateLightTex(worldPosition, sampleColor, normal, specular.rgb, 1.0, specular.a, lightTexture);
    if (cb_hdr) {
        if (cb_exposure >= 0.01f) {
            lightedColor = toneMap(lightedColor, cb_exposure);
        }
        else {
            lightedColor = toneMap(lightedColor);
        }
    }
    float4 correctedColor = getGammaCorrectedColor(lightedColor, cb_gamma);
    return correctedColor;
}
