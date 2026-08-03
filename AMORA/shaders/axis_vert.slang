
struct CameraUBO {
    row_major float4x4 projectionMatrix;
    row_major float4x4 viewMatrix;
    row_major float4x4 inverseMatrix;
}
[[vk_binding(0, 0)]] // Set 0, Binding 0
ConstantBuffer<CameraUBO> ubo;

struct VertexOutput {
    float4 sv_position : SV_Position;
    float2 worldPos;
};

[shader("vertex")]
VertexOutput vertMain(uint vertexId: SV_VertexID) {
    // Full-screen triangle vertices.
    static float2 positions[3] = float2[](
        float2(-1.0, 3.0),
        float2(-1.0, -1.0),
        float2(3.0, -1.0)
    );

    float2 ndc = positions[vertexId];
    float4 world = mul(float4(ndc, 1.0, 1.0), ubo.inverseMatrix);

    VertexOutput output;
    output.sv_position = float4(ndc, 0.0, 1.0);
    output.worldPos = world.xy / world.w;
    return output;
}