#ifndef CONSTANT_BUFFERS_HLSLI_GUARD
#define CONSTANT_BUFFERS_HLSLI_GUARD

cbuffer ambientLightBuffer: register(b0) {
    float3 ambientLightColor;
    float ambientLightStrength;
}

cbuffer PointLightBuffer: register(b1) {
    float3 pointLightColor;
    float pointLightStrength;
    float3 pointLightPosition;
    float specularStrength;
    float3 pointLightattenuations;
    float shinessPower;
}

cbuffer cameraBuffer: register(b2) {
    float3 cameraWorldPos;
}

cbuffer lightsCountBuffer: register(b3) {
    int lightsCount;
}

cbuffer lightsCountBuffer : register(b4) {
    float gamma;
    float exposure;
    bool hdr;
}

#endif // CONSTANT_BUFFERS_HLSLI_GUARD
