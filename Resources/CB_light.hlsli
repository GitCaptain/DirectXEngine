// per object buffers
cbuffer light_object : register(b1) {
    bool lightSource;
}

// texture size is 3 * lightsCnt
// zero row: lightsCnt, ambientLightColor, ambientLightStrength, attenuations
// first row: dynamicLightColor + Strength
// second row: dynamicLightPosition
// Bind this to 10'th registers to avoid intersections
// TODO: figure out how to fix bindings
Texture2D lightInfo: Texture : register(t10);

float4 getLightInfo(Texture2D samplerInput, float2 position) {
    return samplerInput.Load(float3(position, 0));
}

float3 calculateColor(float3 sampleColor, float3 normal, float3 position, bool lightSource=false) {
    int lightsCnt = getLightInfo(lightInfo,  float2(0, 0)).x;
    float3 ambientLightColor = getLightInfo(lightInfo,  float2(1, 0)).rgb;
    float ambientLightStrength = getLightInfo(lightInfo,  float2(2, 0)).x;
    float3 attenuations = getLightInfo(lightInfo,  float2(3, 0)).xyz;
    float dynamicLightAttenuation_a = attenuations.x;
    float dynamicLightAttenuation_b = attenuations.y;
    float dynamicLightAttenuation_c = attenuations.z;
    
    float3 ambientLight = ambientLightColor * ambientLightStrength;
    float3 appliedLight = ambientLight;

    for (int i = 0; i < lightsCnt; i++) {  
        
        float4 dynamicLightSC = getLightInfo(lightInfo,  float2(i, 1));
        float3 dynamicLightColor = dynamicLightSC.xyz;
        float dynamicLightStrength = dynamicLightSC.w;
        float3 dynamicLightPosition = getLightInfo(lightInfo,  float2(i, 2)).xyz;
        
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
    }
    
    float3 finalColor = appliedLight * sampleColor;
    
    return finalColor;
}