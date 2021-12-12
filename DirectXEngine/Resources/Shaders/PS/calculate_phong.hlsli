#ifndef CALCULATE_PHONG_HLSLI_GUARD
#define CALCULATE_PHONG_HLSLI_GUARD

#include "ConstantBuffers.hlsli"

float4 calculateLight(float3 worldPosition, float3 sampleColor, float3 normal) {
    //ambient
    float3 ambientLight = cb_ambientLightColor * cb_ambientLightStrength;

    //diffuse
    float3 vectorToLight = normalize(cb_pointLightPosition - worldPosition);
    float distanceToLight = distance(cb_pointLightPosition, worldPosition);

    float attenuationFactor = 1 / dot(cb_pointLightattenuations, float3(1, distanceToLight, pow(distanceToLight, 2)));

    // we don't lightning objects that are opposite to our light source,
    // but we don't want to make it darker too
    float3 diffuseLightIntensity = max(dot(vectorToLight, normal), 0) * attenuationFactor;
    float3 diffuseLight = diffuseLightIntensity * cb_pointLightStrength * cb_pointLightColor;

    //specular
    float3 viewDirection = normalize(cb_cameraWorldPos - worldPosition);
    float3 reflectDirection = reflect(-vectorToLight, normal);
    float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), cb_shinessPower);
    float3 specularLight = cb_specularStrength * specular * cb_pointLightColor;

    float3 finalColor = sampleColor * (ambientLight + diffuseLight + specularLight);

    return float4(finalColor, 1.0f);
}

#endif // CALCULATE_PHONG_HLSLI_GUARD
