struct VertexOutput {
    float4 sv_position : SV_Position;
    float2 uv;
};

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target
{
    float2 uv = inVert.uv;
    float r = 0.01;
    float g = 0.01;
    float b = 0.009 + 0.05 * uv.x * uv.y;
    return float4(r, g, b, 1.0);
    //return float4(1.0, 1.0, 1.0, 1.0);
}