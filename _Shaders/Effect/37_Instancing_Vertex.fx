#include "00_Context.fx"

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : TEXCOORD;
    float3 Normal : NORMAL;

    matrix Transform : INSTANCE;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

VertexOutput VS_Model(VertexInput input)
{    
    VertexOutput output;
    
    ModelWorldTF = input.Transform;

    output.Position = mul(input.Position, ModelWorldTF);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
           
    output.Normal = mul(input.Normal, (float3x3) ModelWorldTF);
    
    output.Uv = input.Uv;
    
    return output;
}

Texture2D Texture;
SamplerState Samp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS(VertexOutput input) : SV_Target
{
    float NdotL = dot((input.Normal), -LightDirection);

    float3 color = Texture.Sample(Samp, input.Uv).rgb * NdotL;
    return float4(color.rgb, 1);
}


technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Model()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}