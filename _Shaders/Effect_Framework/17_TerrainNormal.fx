matrix World;
matrix View;
matrix Projection;

struct VertexInput
{
    float4 Position : POSITION;
    float3 Normal : Normal;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float3 Normal : Normal;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.Normal = mul(input.Normal, (float3x3)World);
    
    return output;
}

float3 LightDirection = float3(-1,-1,1);
float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float3 color = dot(-normalize(LightDirection), normal);
    return float4(color, 1.0f);
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}