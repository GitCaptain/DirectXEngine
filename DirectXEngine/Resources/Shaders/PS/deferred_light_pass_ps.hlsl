#include "LightConstantBuffers.hlsli"

Texture2D positionTex:        register(t0);
Texture2D colorTex:           register(t1);
Texture2D normalTex:          register(t2);
SamplerState perPixelSampler: register(s0);


float4 main(float4 screenPos: SV_POSITION) : SV_TARGET {

    float3 texCoord = float3(screenPos.xy, 0);
    float3 sampleColor = colorTex.Load(texCoord);
    float3 normal = normalTex.Load(texCoord);
    float3 worldPosition = positionTex.Load(texCoord);

    //ambient
    float3 ambientLight = ambientLightColor * ambientLightStrength;

    //diffuse
    float3 vectorToLight = normalize(dynamicLightPosition - worldPosition);
    float distanceToLight = distance(dynamicLightPosition, worldPosition);

    float attenuationFactor = 1 / (dynamicLightAttenuation_a +
                                   dynamicLightAttenuation_b * distanceToLight +
                                   dynamicLightAttenuation_c * pow(distanceToLight, 2));

    // we don't lightning objects that are opposite to our light source,
    // but we don't want to make it darker too
    float3 diffuseLightIntensity = max(dot(vectorToLight, normal), 0) * attenuationFactor;
    float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;

    //specular
    float3 viewDirection = normalize(cameraWorldPos - worldPosition);
    float3 reflectDirection = reflect(-vectorToLight, normal);
    float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), shinessPower);
    float3 specularLight = specularStrength * specular * dynamicLightColor;

    float3 finalColor = sampleColor * (ambientLight + diffuseLight + specularLight);

    return float4(finalColor, 1.0f);
}
