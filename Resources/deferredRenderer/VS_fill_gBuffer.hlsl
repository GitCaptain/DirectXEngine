cbuffer MatrixBuffer {
    matrix worldMatrix;
    matrix viewProjectionMatrix;
};

struct VS_INPUT {
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

struct VS_OUTPUT {
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPosition : WORLD_POSITION;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    
    float4x4 wvpMatrix = worldMatrix * viewProjectionMatrix;
    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
    
    // Calculate the position of the vertex against the world matrix for the pixel shader.
    output.outWorldPosition = mul(float4(input.inPos, 1.0f), worldMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.outTexCoord = input.inTexCoord;
    
    // Calculate the normal vector against the world matrix only.
    output.outNormal = normalize(mul(input.inNormal, (float3x3) worldMatrix));

    return output;
}