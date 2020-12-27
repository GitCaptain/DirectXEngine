struct PS_INPUT {
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPosition : WORLD_POSITION;
};

Texture2D shaderTexture : register(t0);

SamplerState objSamplerState : register(s0);

struct PS_OUTPUT {
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
    float4 position : SV_Target2;
};

PS_OUTPUT main(PS_INPUT input) : SV_TARGET {
    PS_OUTPUT output;

    // Sample the color from the texture and store it for output to the render target.
    output.color = shaderTexture.Sample(objSamplerState, input.inTexCoord);

    // Store the normal for output to the render target.
    output.normal = float4(input.inNormal, 1.0f);

    // Store the world position for output to the render target.
    output.position = float4(input.inWorldPosition, 1);

    return output;
}

