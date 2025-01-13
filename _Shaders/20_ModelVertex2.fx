#include "00_Context.fx"
#include "00_Material.fx"

#define MAX_MODEL_TRANSFORM 250
cbuffer CB_ModelBones
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORM];
    
    uint BoneIndex;
};

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float4 Color : Color;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 Indices : BlendIndices;
    float4 Weight : BlendWeights;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

VertexOutput VS(VertexInput input)
{    
    VertexOutput output;
    
    World = mul(BoneTransforms[BoneIndex], World);
    
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.Normal = mul(input.Normal, (float3x3)World);
    
    output.Uv = input.Uv;
    
    return output;
}

SamplerState Samp;
float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float light = dot(-LightDirection, normal);
    
    float3 color = MaterialMaps[MATERIAL_TEXTURE_DIFFUSE].Sample(Samp, input.Uv).rgb;
    color *= light;
    
    return float4(color, 1.0f);
}

RasterizerState FillMode_Wireframe
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
        SetRasterizerState(FillMode_Wireframe);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}