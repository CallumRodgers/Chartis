
static float2 positions[3] = float2[](
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5)
);

static float3 colors[3] = float3[](
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
);

struct UniformBufferObject {
    row_major float4x4 projectionMatrix;
    row_major float4x4 viewMatrix;
}
[[vk_binding(0, 0)]] // Set 0, Binding 0
ConstantBuffer<UniformBufferObject> ubo;

struct VertexOutput {
    float4 color;
    float4 sv_position : SV_Position;
};

[shader("vertex")]
VertexOutput vertMain(uint vid : SV_VertexID) {
    VertexOutput output;

    output.sv_position = mul(mul(float4(positions[vid], 0.0, 1.0), ubo.viewMatrix), ubo.projectionMatrix);
    output.color = float4(colors[vid], 1.0);
    return output;
}