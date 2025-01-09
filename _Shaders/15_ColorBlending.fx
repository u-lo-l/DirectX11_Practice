matrix World;
matrix View;
matrix Projection;
Texture2D Map;
float LerpRate;

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
    float4 Color1 = Map.Sample(Samp, input.UV);
    float4 Color2 = input.Color;
    
    return lerp(Color1, Color2, LerpRate);
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}