#ifndef TONE_MAPPING_HLSLI_GUARD
#define TONE_MAPPING_HLSLI_GUARD

float4 toneMap(float4 hdrColor) {
    float3 colorPart = hdrColor.rgb;
    float3 mappedColor = colorPart / (colorPart + float3(1.0, 1.0, 1.0));
    return float4(mappedColor, hdrColor.a);
}

float4 toneMap(float4 hdrColor, float exposure) {
    float3 colorPart = hdrColor.rgb;
    float3 mapped = float3(1.0, 1.0, 1.0) - exp(-hdrColor * exposure);
    return float4(mapped, hdrColor.a);
}

#endif //TONE_MAPPING_HLSLI_GUARD
