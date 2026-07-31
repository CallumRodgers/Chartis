struct VertexOutput {
    float4 sv_position : SV_Position;
    float2 uv;
};

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target
{
    float2 uv = inVert.uv;
    float r = 0.02;
    float g = 0.02;
    float b = 0.02 + 0.1 * uv.x * uv.y;
    return float4(r, g, b, 1.0);
}