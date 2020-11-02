cbuffer alphaBuffer : register(b0) {
    float alpha;
}

struct PS_INPUT {
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : Texture : register(t0);
SamplerState objSamplerState : Sampler : register(s0);


float4 main(PS_INPUT input) : SV_TARGET {
    float3 pixelColor = objTexture.Sample(objSamplerState, input.inTexCoord);
	return float4(pixelColor, alpha);
}