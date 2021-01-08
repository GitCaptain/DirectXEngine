// per object buffers
cbuffer light_object : register(b1) {
    bool lightSource;
}

// texture size is 3 * lightsCnt
// zero row: TextureWidth(max lights cnt) +  lightsCnt, ambientLightColor, ambientLightStrength, attenuations
// first row: dynamicLightColor + Strength
// second row: dynamicLightPosition
// Bind this to 10'th registers to avoid intersections
// TODO: figure out how to fix bindings
Texture2D lightInfo: Texture : register(t10);
SamplerState lightSampler : Sampler : register(s10);

float2 getTextCoords(float y, float x, float width, float height = 3) {
    // return coords in range [0, 1]
    return float2(x / width, y / height);
}
    
float3 calculateColor(float3 sampleColor, float3 normal, float3 position, bool lightSource=false) {

    const float MAX_LIGHTS_CNT = lightInfo.Sample(lightSampler, float2(0, 0)).x; // texture width
    int lightsCnt = lightInfo.Sample(lightSampler, float2(0, 0)).y;
    float3 ambientLightColor = lightInfo.Sample(lightSampler, getTextCoords(0, 1, MAX_LIGHTS_CNT)).rgb;
    float ambientLightStrength = lightInfo.Sample(lightSampler, getTextCoords(0, 2, MAX_LIGHTS_CNT)).x;
    float3 attenuations = lightInfo.Sample(lightSampler, getTextCoords(0, 3, MAX_LIGHTS_CNT)).xyz;
    float dynamicLightAttenuation_a = attenuations.x;
    float dynamicLightAttenuation_b = attenuations.y;
    float dynamicLightAttenuation_c = attenuations.z;

    float3 ambientLight = ambientLightColor * ambientLightStrength;
    float3 appliedLight = ambientLight;
    
    for (int i = 0; i < lightsCnt; i++) {  
        
        float4 dynamicLightSC = lightInfo.Sample(lightSampler, getTextCoords(1, i, MAX_LIGHTS_CNT));
        float3 dynamicLightColor = dynamicLightSC.xyz;
        float dynamicLightStrength = dynamicLightSC.w;
        float3 dynamicLightPosition = lightInfo.Sample(lightSampler, getTextCoords(2, i, MAX_LIGHTS_CNT)).xyz;
        
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