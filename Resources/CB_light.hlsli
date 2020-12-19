cbuffer lightBuffer : register(b0) {
    float3 ambientLightColor;
    float ambientLightStrength;
    
    float3 dynamicLightColor;
    float dynamicLightStrength;
    
    float3 dynamicLightPosition;
    
    float dynamicLightAttenuation_a;
    float dynamicLightAttenuation_b;
    float dynamicLightAttenuation_c;
}

// TODO: use this function in other pixelshaders? 
// then no need to use `pixelshader_nolight`
// just add new lightSource constant to lightBuffer
float3 calculateColor(float3 sampleColor, float3 normal, float3 position, bool lightSource=false) {
    float3 ambientLight = ambientLightColor * ambientLightStrength;
    
    float3 appliedLight = ambientLight;
         
    float3 diffuseLightIntensity;
    float distanceToLight;
    if (lightSource) {
        distanceToLight = 1;
        diffuseLightIntensity = normalize(float3(1, 1, 1));
    }
    else {
        float3 vectorToLight = normalize(dynamicLightPosition - position);
        // we don't lightning objects that are opposite to our light source,
        // but we don't want to make it darker too
        diffuseLightIntensity = max(dot(vectorToLight, normal), 0);
        distanceToLight = distance(dynamicLightPosition, position);
    }
    
    float attenuationFactor = 1.0 / (dynamicLightAttenuation_a +
                                     dynamicLightAttenuation_b * distanceToLight +
                                     dynamicLightAttenuation_c * pow(distanceToLight, 2));
       
    diffuseLightIntensity *= attenuationFactor;

    float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;
    
    appliedLight += diffuseLight;
    
    float3 finalColor = sampleColor * appliedLight;
    return finalColor;
}