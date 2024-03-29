#ifndef CALCULATE_PHONG_TEX_HLSLI_GUARD
#define CALCULATE_PHONG_TEX_HLSLI_GUARD

#include "ConstantBuffers.hlsli"


float loadTexelAt(int pos, Texture1D srcTexture) {
    return srcTexture.Load(int2(pos, 0));
}

float3 loadTexel3At(int pos, Texture1D srcTexture) {
    float3 res;
    res.x = loadTexelAt(pos, srcTexture);
    res.y = loadTexelAt(pos + 1, srcTexture);
    res.z = loadTexelAt(pos + 2, srcTexture);
    return res;
}

float4 calculateLightTex(
    float3 worldPosition,
    float3 sampleColor,
    float3 normal,
    float3 specularColor,
    float specularStrength,
    float specularPower,
    Texture1D lightTexture
) {

    //ambient
    float3 ambientLight = cb_ambientLightColor * cb_ambientLightStrength;

    float3 diffuseLight = float3(0, 0, 0);
    float3 specularLight = float3(0, 0, 0);
    [loop] for (int i = 0, texCoord = 0; i < cb_lightsCount; i++, texCoord += 10) {
        //diffuse
        float3 pointLightColor = loadTexel3At(texCoord, lightTexture);
        float3 pointLightAttenuations = loadTexel3At(texCoord + 3, lightTexture);
        float3 pointLightPosition = loadTexel3At(texCoord + 6, lightTexture);
        float pointLightStrength = loadTexelAt(texCoord + 9, lightTexture);

        float3 vectorToLight = normalize(pointLightPosition - worldPosition);
        float distanceToLight = distance(pointLightPosition, worldPosition);

        float attenuationFactor = dot(pointLightAttenuations, float3(1, distanceToLight, pow(distanceToLight, 2)));

        // attenuationFactor should always be greater than zero
        if (attenuationFactor > 0) {
            // we don't lightning objects that are opposite to our light source,
            // but we don't want to make it darker too
            float3 diffuseLightIntensity = max(dot(vectorToLight, normal), 0) / attenuationFactor;
            diffuseLight += pointLightColor * diffuseLightIntensity * pointLightStrength;
        }
        //specularColor
        float3 viewDirection = normalize(cb_cameraWorldPos - worldPosition);
        float3 reflectDirection = reflect(-vectorToLight, normal);
        float specularIntensity = pow(max(dot(viewDirection, reflectDirection), 0.0), specularPower);
        specularLight += specularStrength * specularIntensity * specularColor;

    }

    float3 finalColor = sampleColor * (ambientLight + diffuseLight + specularLight);

    return float4(finalColor, 1.0f);
}

#endif // CALCULATE_PHONG_TEX_HLSLI_GUARD
