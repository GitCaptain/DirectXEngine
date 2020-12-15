cbuffer perObjectBuffer : register(b0) {
    row_major float4x4 viewProjectionMatrix;
	row_major float4x4 worldMatrix;
};

struct VS_INPUT {
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
	float3 inNormal : NORMAL;
};

struct VS_OUTPUT {
    float4 outPosition : SV_POSITION;
    float2 outTexCoord: TEXCOORD;
	float3 outNormal : NORMAL;
	float3 outWorldPosition: WORLD_POSITION;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    float4x4 worldViewProjection = mul(worldMatrix, viewProjectionMatrix);
    output.outPosition = mul(float4(input.inPos, 1.0f), worldViewProjection);
    output.outTexCoord = input.inTexCoord;
	output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), worldMatrix));
	output.outWorldPosition = mul(float4(input.inPos, 1.0f), worldMatrix);
    return output;
}