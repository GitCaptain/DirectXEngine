#pragma once
#include "ConstantBuffers.hlsli"

float4 calculateLight(float3 worldPosition, float3 sampleColor, float3 normal) {
    //ambient
    float3 ambientLight = ambientLightColor * ambientLightStrength;

    //diffuse
    float3 vectorToLight = normalize(pointLightPosition - worldPosition);
    float distanceToLight = distance(pointLightPosition, worldPosition);

    float attenuationFactor = 1 / dot(pointLightattenuations, float3(1, distanceToLight, pow(distanceToLight, 2)));

    // we don't lightning objects that are opposite to our light source,
    // but we don't want to make it darker too
    float3 diffuseLightIntensity = max(dot(vectorToLight, normal), 0) * attenuationFactor;
    float3 diffuseLight = diffuseLightIntensity * pointLightStrength * pointLightColor;

    //specular
    float3 viewDirection = normalize(cameraWorldPos - worldPosition);
    float3 reflectDirection = reflect(-vectorToLight, normal);
    float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), shinessPower);
    float3 specularLight = specularStrength * specular * pointLightColor;

    float3 finalColor = sampleColor * (ambientLight + diffuseLight + specularLight);

    return float4(finalColor, 1.0f);
}
