
struct VertexOutput {
    float4 sv_position : SV_Position;
    float2 worldPos;
};

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {

    float2 gridDev = fwidth(inVert.worldPos);
    float2 grid = abs(frac(inVert.worldPos - 0.5) - 0.5) / gridDev;
    float lineDist = min(grid.x, grid.y);
    float gridAlpha = 1.0 - min(lineDist, 1.0);

    float2 axisDist = abs(inVert.worldPos) / gridDev;
    float xAxis = 1.0 - min(axisDist.y, 1.0);
    float yAxis = 1.0 - min(axisDist.x, 1.0);

    float totalAlpha = max(gridAlpha * 0.05, max(xAxis, yAxis));

    if (totalAlpha < 0.01) discard;

    return float4(1.0, 1.0, 1.0, totalAlpha);
}