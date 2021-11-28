#include "ConstantBuffers.hlsli"

float4 calculateLightTex(float3 worldPosition, float3 sampleColor, float3 normal, Texture2D lightTexture) {

    //ambient
    float3 ambientLight = ambientLightColor * ambientLightStrength;

    float3 diffuseLight = float3(0, 0, 0);
    float3 specularLight = float3(0, 0, 0);
    [loop] for (int i = 0; i < lightsCount; i++) {
        //diffuse
        float3 pointLightColor = lightTexture.Load(int3(0, i, 0)).xyz;
        float3 pointLightattenuations = lightTexture.Load(int3(1, i, 0)).xyz;
        float4 tmp = lightTexture.Load(int3(2, i, 0));
        float3 pointLightPosition = tmp.xyz;
        float pointLightStrength = tmp.w;

        float3 vectorToLight = normalize(pointLightPosition - worldPosition);
        float distanceToLight = distance(pointLightPosition, worldPosition);

        float attenuationFactor = 1 / dot(pointLightattenuations, float3(1, distanceToLight, pow(distanceToLight, 2)));

        // we don't lightning objects that are opposite to our light source,
        // but we don't want to make it darker too
        float3 diffuseLightIntensity = max(dot(vectorToLight, normal), 0) * attenuationFactor;
        diffuseLight += diffuseLightIntensity * pointLightStrength * pointLightColor;

        ////specular
        //float3 viewDirection = normalize(cameraWorldPos - worldPosition);
        //float3 reflectDirection = reflect(-vectorToLight, normal);
        //float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), shinessPower);
        //specularLight += specularStrength * specular * pointLightColor;

    }

    float3 finalColor = sampleColor * (ambientLight + diffuseLight + specularLight);

    return float4(finalColor, 1.0f);
}
