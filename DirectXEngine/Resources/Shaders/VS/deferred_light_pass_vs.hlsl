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
    float4 outPosition : SV_POSITION;
    float2 outTexCoord: TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPosition: WORLD_POSITION;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType DeferredVertexShader(VertexInputType input)
{
    PixelInputType output;


    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);

    // Normalize the normal vector.
    output.normal = normalize(output.normal);

    return output;
}
