cbuffer perObjectBuffer : register(b0) {
    row_major float4x4 worldViewProjectionMatrix;
    row_major float4x4 worldMatrix;
};

struct VS_INPUT {
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

struct VS_OUTPUT {
    float4 sv_pos: SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.sv_pos = mul(float4(input.inPos, 1.0f), worldViewProjectionMatrix);
    return output;
}
