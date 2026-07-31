
struct VertexOutput {
    float4 color;
    float4 sv_position : SV_Position;
};

struct UniformBufferObject {
    float4x4 projectionMatrix;
    float4x4 viewMatrix;
}
[[vk::binding(0, 0)]] // Set 0, Binding 0
ConstantBuffer<UniformBufferObject> ubo;

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
    return inVert.color;
}