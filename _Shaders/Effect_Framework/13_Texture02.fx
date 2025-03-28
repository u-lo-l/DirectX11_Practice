matrix World;
matrix View;
matrix Projection;
Texture2D Map;

struct VertexInput
{
    float4 Position : POSITION;
    float2 UV : Uv;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 UV : Uv;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.UV = input.UV;

    return output;
}

uint Filter;

SamplerState Sampler_Point
{
    Filter = MIN_MAG_MIP_POINT;
};

SamplerState Sampler_Linear
{
    Filter = MIN_MAG_MIP_LINEAR;
};


float4 PS(VertexOutput input) : SV_Target
{
    if (Filter == 0)
        return Map.Sample(Sampler_Point, input.UV);
    else if (Filter == 1)
        return Map.Sample(Sampler_Linear, input.UV);
    else
        return float4(1, 1, 1, 1);
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}