#include "ConstantBuffers.hlsli"

struct PS_INPUT {
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPosition: WORLD_POSITION;
};

Texture2D objTexture: register(t0);
Texture2D specTexture: register(t1);
SamplerState objSamplerState: register(s0);

struct PS_OUTPUT {
    float4 worldPosition: SV_Target0;
    float4 color : SV_Target1;
    float4 normal : SV_Target2;
    float4 specular : SV_Target3;
};

PS_OUTPUT main(PS_INPUT input) : SV_TARGET {
    PS_OUTPUT output;

    output.worldPosition = float4(input.inWorldPosition, 1.0f);
    output.color = objTexture.Sample(objSamplerState, input.inTexCoord);
    output.normal = float4(input.inNormal, 1.0f);
    // multiply specular by strength now, to avoid this in light pass
    output.specular = specTexture.Sample(objSamplerState, input.inTexCoord) * cb_specularStrength;
    output.specular.a = cb_shinessPower; // save specular power to alpha channel, to use in light pass
    return output;
}
