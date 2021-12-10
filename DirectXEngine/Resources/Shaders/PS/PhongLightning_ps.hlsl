#include "calculate_phong_tex.hlsli"
#include "gamma_correction.hlsli"

struct PS_INPUT {
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPosition: WORLD_POSITION;
};

Texture2D objTexture:         register(t0);
Texture1D lightTexture:       register(t1);
SamplerState objSamplerState: register(s0);

float4 main(PS_INPUT input) : SV_TARGET {
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    float4 lightedColor = calculateLightTex(input.inWorldPosition, sampleColor, input.inNormal, lightTexture);
    float4 correctedColor = getGammaCorrectedColor(lightedColor, gamma);
    return correctedColor;
}
