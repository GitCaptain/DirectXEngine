#include "LightConstantBuffers.hlsli"

Texture2D positionTex: Texture: register(t0);
Texture2D colorTex:    Texture: register(t1);
Texture2D normalTex:   Texture: register(t2);
SamplerState perPixelSampler : Sampler: register(s0);

struct PS_INPUT {
    float4 sv_pos: SV_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET {
    float4 texCoord = input.sv_pos;
    float3 sampleColor = colorTex.Sample(perPixelSampler, texCoord.xy);
    float3 normal = normalTex.Sample(perPixelSampler, texCoord.xy);
    float3 worldPosition = positionTex.Sample(perPixelSampler, texCoord.xy);

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
