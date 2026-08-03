
struct VertexOutput {
    float4 sv_position : SV_Position;
    float2 uv;
};

[shader("vertex")]
VertexOutput vertMain(uint vid: SV_VertexID) {
    static const float2 positions[3] = float2[](
        float2(-1.0, 3.0),
        float2(-1.0, -1.0),
        float2(3.0, -1.0)
    );

    VertexOutput output;
    output.sv_position = float4(positions[vid], 0.0, 1.0);
    output.uv = (positions[vid] + 1.0) / 2.0;
    return output;
}