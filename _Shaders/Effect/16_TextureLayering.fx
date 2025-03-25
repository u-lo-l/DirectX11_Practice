matrix World;
matrix View;
matrix Projection;
Texture2D Map[3];

struct VertexInput
{
    float4 Position : POSITION;
    float2 UV : Uv;
    float4 Color : Color;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 UV : Uv;
    float4 Color : Color;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.UV = input.UV;
    output.Color = input.Color;
    
    return output;
}

SamplerState Samp;

float4 PS(VertexOutput input) : SV_Target
{
    float4 Color0 = Map[0].Sample(Samp, input.UV);
    float4 Color1 = Map[1].Sample(Samp, input.UV);
    float4 Weight = Map[2].Sample(Samp, input.UV);
    
    return lerp(Color0, Color1, Weight);
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}