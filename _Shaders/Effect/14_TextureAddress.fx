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

uint Address;
float Tiling;
float Offset;

SamplerState Sampler_Wrap
{
    AddressU = WRAP;
    AddressV = WRAP;
};

SamplerState Sampler_Mirror
{
    AddressU = MIRROR;
    AddressV = MIRROR;
};

SamplerState Sampler_Clamp
{
    AddressU = CLAMP;
    AddressV = CLAMP;
};

SamplerState Sampler_Border
{
    AddressU = BORDER;
    AddressV = BORDER;
    BORDERCOLOR = float4(0, 0, 1, 1);
};


float4 PS(VertexOutput input) : SV_Target
{
    if (Address == 0)
        return Map.Sample(Sampler_Wrap, input.UV);
    else if (Address == 1)
        return Map.Sample(Sampler_Mirror, input.UV);
    else if (Address == 2)
        return Map.Sample(Sampler_Clamp, input.UV);
    else if (Address == 3)
        return Map.Sample(Sampler_Border, input.UV);
    else
        return float4(1, 1, 1, 1);
}

float4 PS_Tiling(VertexOutput input) : SV_Target
{
    float2 uv = input.UV;
    uv *= Tiling;
    return Map.Sample(Sampler_Wrap, uv);
    
}

float4 PS_Offset(VertexOutput input) : SV_Target
{
    float2 uv = input.UV;
    uv += Offset;
    return Map.Sample(Sampler_Wrap, uv);
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
        SetPixelShader(CompileShader(ps_5_0, PS_Tiling()));
    }
    pass P2
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Offset()));
    }

}