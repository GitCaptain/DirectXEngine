#ifndef GAMMA_CORRECTION_HLSLI_GUARD
#define GAMMA_CORRECTION_HLSLI_GUARD

float4 getGammaCorrectedColor(float4 color, float gammaCoef) {
    float invertedGamma = 1 / gammaCoef;
    float3 correctedColor = pow(color.xyz, float3(invertedGamma, invertedGamma, invertedGamma));
    return float4(correctedColor, color.a);
}

#endif // GAMMA_CORRECTION_HLSLI_GUARD
