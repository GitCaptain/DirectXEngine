
// if you change variable here
// you also must change it ConstantBufferTypes
// and recompile it!
#define MAXIMUM_LIGHTS 16 / 4
/*
// per frame buffers
cbuffer light_frame : register(b0) {
    float3 dynamicLightColor[MAXIMUM_LIGHTS];
    float dynamicLightStrength[MAXIMUM_LIGHTS];
    
    float3 dynamicLightPosition[MAXIMUM_LIGHTS];
    
    float3 ambientLightColor;
    float ambientLightStrength;
    
    float dynamicLightAttenuation_a;
    float dynamicLightAttenuation_b;
    float dynamicLightAttenuation_c;
    
    int lightsCnt;
}

// per object buffers
cbuffer light_object : register(b1) {
    bool lightSource;
}
*/

// texture size is 3 * MAXIMUM_LIGHTS
// first row: lightsCnt, ambientLightColor, ambientLightStrength, attenuations
// second row: dinamycLightColor + Strength
// third row: dynamicLightPosition
Texture2D lightInfo: Texture : register(t1);
SamplerState lightSampler : Sampler : register(s1);
    
// TODO: use this function in other pixelshaders? 
// then no need to use `pixelshader_nolight`
// just add new lightSource constant to lightBuffer
float3 calculateColor(float3 sampleColor, float3 normal, float3 position, bool lightSource=false) {
    
    float3 ambientLightColor = lightInfo.Sample(lightSampler, float2(0, 0));
    float3 ambientLightColor = lightInfo.Sample(lightSampler, float2(0, 0));
    float3 ambientLightColor = lightInfo.Sample(lightSampler, float2(0, 0));
    float3 ambientLightColor = lightInfo.Sample(lightSampler, float2(0, 0));
    
    float3 finalColor = float3(0, 1, 0);
    for (int i = 0; i < lightsCnt; i++) {
       
        float3 ambientLight = ambientLightColor * ambientLightStrength;
    
        float3 appliedLight = ambientLight;
         
        float3 diffuseLightIntensity;
        float distanceToLight;
        if (lightSource) {
            distanceToLight = 1;
            diffuseLightIntensity = normalize(float3(1, 1, 1));
        }
        else {
            float3 vectorToLight = normalize(dynamicLightPosition[i] - position);
            // we don't lightning objects that are opposite to our light source,
            // but we don't want to make it darker too
            diffuseLightIntensity = max(dot(vectorToLight, normal), 0);
            distanceToLight = distance(dynamicLightPosition[i], position);
        }
    
        float attenuationFactor = 1.0 / (dynamicLightAttenuation_a +
                                     dynamicLightAttenuation_b * distanceToLight +
                                     dynamicLightAttenuation_c * pow(distanceToLight, 2));
       
        diffuseLightIntensity *= attenuationFactor;

        float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength[i] * dynamicLightColor[i];
    
        appliedLight += diffuseLight;
    
        finalColor += sampleColor * appliedLight;
    }
    return finalColor;
}