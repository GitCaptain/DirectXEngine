cbuffer MatrixBuffer {
    matrix worldMatrix;
    matrix viewProjectionMatrix;
};

struct VS_Input {
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VS_Output {
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

VS_Output main(VS_Input input) {
    VS_Output output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    float4x4 wvpMatrix = worldMatrix * viewProjectionMatrix;
    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, wvpMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    // Calculate the normal vector against the world matrix only.
    output.normal = normalize(mul(input.normal, (float3x3) worldMatrix));

    return output;
}