cbuffer lightBuffer: register(b0) {
    float3 ambientLightColor;
    float ambientLightStrength;

    float3 dynamicLightColor;
    float dynamicLightStrength;

    float3 dynamicLightPosition;
    float specularStrength;

    float shinessPower;
    float dynamicLightAttenuation_a;
    float dynamicLightAttenuation_b;
    float dynamicLightAttenuation_c;
}

cbuffer cameraBuffer: register(b1) {
    float3 cameraWorldPos;
}
