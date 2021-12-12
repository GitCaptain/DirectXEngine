#ifndef CONSTANT_BUFFERS_HLSLI_GUARD
#define CONSTANT_BUFFERS_HLSLI_GUARD

cbuffer ambientLightBuffer: register(b0) {
    float3 cb_ambientLightColor;
    float cb_ambientLightStrength;
}

cbuffer PointLightBuffer: register(b1) {
    float3 cb_pointLightColor;
    float cb_pointLightStrength;
    float3 cb_pointLightPosition;
    float cb_specularStrength;
    float3 cb_pointLightattenuations;
    float cb_shinessPower;
}

cbuffer cameraBuffer: register(b2) {
    float3 cb_cameraWorldPos;
}

cbuffer lightsCountBuffer: register(b3) {
    int cb_lightsCount;
}

cbuffer lightsCountBuffer : register(b4) {
    float cb_gamma;
    float cb_exposure;
    bool cb_hdr;
}

#endif // CONSTANT_BUFFERS_HLSLI_GUARD
