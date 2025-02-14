matrix World;
matrix View;
matrix Projection;

uint Index;

struct VertexInput
{
    float4 Position : POSITION;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    if (Index == 0)
        return float4(1, 0, 0, 1);
    else if (Index == 1)
        return float4(0, 1, 0, 1);
    else if (Index == 2)
        return float4(0, 0, 1, 1); // r,g,b,a
    return float4(0, 0, 0, 1);
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}