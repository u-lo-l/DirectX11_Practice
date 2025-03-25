#include "00_Context.fx"

#define MATERIAL_TEXTURE_DIFFUSE 0
#define MATERIAL_TEXTURE_NORMAL 1
#define MATERIAL_TEXTURE_SPECULAR 2
#define MAX_MATERIAL_TEXTURE_COUNT 3
Texture2D MaterialMaps[MAX_MATERIAL_TEXTURE_COUNT];

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float4 Color : Color;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 Indices : BlendIndices;
    float4 weights : BlendWeights;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, ModelWorldTF);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    return output;
}

SamplerState Samp;
float4 PS(VertexOutput input) : SV_Target
{
	return float4(1.f, 1.f, 1.f, 1.f);
}

RasterizerState FillMode_WireFrame
{
    FillMode = Wireframe;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
    pass P1
    {
        SetRasterizerState(FillMode_WireFrame);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}