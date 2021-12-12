#include "calculate_phong_tex.hlsli"
#include "gamma_correction.hlsli"
#include "tone_mapping.hlsli"

struct PS_INPUT {
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPosition: WORLD_POSITION;
};

Texture2D objTexture:         register(t0);
Texture2D specTexture:         register(t1);
Texture1D lightTexture:       register(t2);
SamplerState objSamplerState: register(s0);

float4 main(PS_INPUT input) : SV_TARGET {
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    float3 specularColor = specTexture.Sample(objSamplerState, input.inTexCoord);
    float4 lightedColor = calculateLightTex(
        input.inWorldPosition,
        sampleColor,
        input.inNormal,
        specularColor,
        cb_specularStrength,
        cb_shinessPower,
        lightTexture
    );
    if (cb_hdr) {
        if(cb_exposure >= 0.01f) {
            lightedColor = toneMap(lightedColor, cb_exposure);
        }
        else {
            lightedColor = toneMap(lightedColor);
        }
    }
    float4 correctedColor = getGammaCorrectedColor(lightedColor, cb_gamma);
    return correctedColor;
}
