struct PS_Input {
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

Texture2D shaderTexture : register(t0);
SamplerState objSamplerState : register(s0);

struct PS_Output {
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
};

PS_Output main(PS_Input input) : SV_TARGET {
    PS_Output output;

    // Sample the color from the texture and store it for output to the render target.
    output.color = shaderTexture.Sample(objSamplerState, input.tex);
	
    // Store the normal for output to the render target.
    output.normal = float4(input.normal, 1.0f);

    return output;
}

