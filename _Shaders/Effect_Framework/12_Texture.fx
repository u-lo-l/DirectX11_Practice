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

SamplerState Sampler;

float4 PS(VertexOutput input) : SV_Target
{
    float4 Color = Map.Sample(Sampler, input.UV);
    return Color;
}

float4 PS02(VertexOutput input) : SV_Target
{
    float4 Color = Map.Sample(Sampler, input.UV);

    if (input.UV.x < 0.5f)
        Color = float4(1, 0, 0, 1); 
    return Color;
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS02()));
    }
}