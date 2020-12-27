#include "../CB_light.hlsli"

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D positionTexture : register(t2);

SamplerState samplerState : register(s0);

cbuffer LightBuffer {
    float3 lightDirection;
    float padding;
};

struct PS_Input {
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PS_Input input) : SV_TARGET {
    
    // Sample the colors from the color render texture using the point sampler at this texture coordinate location.
    float4 sampleColor = colorTexture.Sample(samplerState, input.tex);

    // Sample the normals from the normal render texture using the point sampler at this texture coordinate location.
    float4 normal = normalTexture.Sample(samplerState, input.tex);
    
    float4 position = positionTexture.Sample(samplerState, input.tex);

    float3 outputColor = calculateColor(sampleColor.rgb, normal.xyz, position.xyz, lightSource);

    return float4(outputColor, 1.0);
}