cbuffer lightBuffer: register(b0) {
    float3 ambientLightColor;
    float ambientLightStrength;
}

struct PS_INPUT {
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : Texture : register(t0);
SamplerState objSamplerState : Sampler : register(s0);


float4 main(PS_INPUT input) : SV_TARGET {
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
	
    float3 ambientLight = ambientLightColor * ambientLightStrength;
    
	float3 finalColor = sampleColor * ambientLight;
    
    return float4(finalColor, 1.0f);
}