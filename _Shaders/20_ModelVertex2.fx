#include "00_Context.fx"
#include "00_Material.fx"

#define MaxModelTransforms 256
cbuffer CB_ModelBones
{
    matrix OffsetMatrix[MaxModelTransforms];
    matrix BoneTransforms[MaxModelTransforms];
    uint BoneIndex;
}

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
    float2 Uv : Uv;
	float3 Normal : Normal;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    ModelWorldTF = mul(BoneTransforms[BoneIndex], ModelWorldTF);

    output.Position = mul(input.Position, ModelWorldTF);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
	output.Normal = mul(input.Normal, (float3x3)ModelWorldTF);

    output.Uv = input.Uv;

    return output;
}

SamplerState Samp;
float4 PS(VertexOutput input) : SV_Target
{
	float3 normal = normalize(input.Normal);
	float Light = dot(-LightDirection, normal);
	float3 Color = MaterialMaps[MATERIAL_TEXTURE_DIFFUSE].Sample(Samp, input.Uv).rgb;
    Color *= Light;
	return float4(Color, 1.0f);
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