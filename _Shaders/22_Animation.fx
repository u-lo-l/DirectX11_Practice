#include "00_Context.fx"
#include "00_Material.fx"

#define MAX_MODEL_TRANSFORM 256
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

struct AnimationFrame
{
    uint Clip; // 몇 번 째 clip인지
    uint CurrentFrame; // 그 clip에서 몇 번째 Frame인지지

    float2 Padding;
};

cbuffer CB_AnimationFrame
{
    AnimationFrame KeyFrameData;
};

Texture2DArray ClipsTFMap;

void SetAnimationWorld(inout matrix world, VertexInput input)
{
    int Indices[4] = { input.Indices.x, input.Indices.y, input.Indices.z, input.Indices.w };
    float Weights[4] = { input.Weight.x, input.Weight.y, input.Weight.z, input.Weight.w };
    float weightSum = Weights[0] + Weights[1] + Weights[2] + Weights[3];
    if (weightSum > 0)
    {
        for (int i = 0; i < 4; i++)
        {
            Weights[i] /= weightSum;
        }
    }
    uint Clip = 0;
    uint CurrentFrame = 60;

    float4 ClipTransform[4];

    matrix current = 0;
    matrix transform = 0;

    for(int i = 0 ; i < 4 ; i++)
    {
        ClipTransform[0] = ClipsTFMap.Load(int4(Indices[i] * 4 + 0, CurrentFrame, Clip, 0));
        ClipTransform[1] = ClipsTFMap.Load(int4(Indices[i] * 4 + 1, CurrentFrame, Clip, 0));
        ClipTransform[2] = ClipsTFMap.Load(int4(Indices[i] * 4 + 2, CurrentFrame, Clip, 0));
        ClipTransform[3] = ClipsTFMap.Load(int4(Indices[i] * 4 + 3, CurrentFrame, Clip, 0));
        current = matrix(ClipTransform[0], ClipTransform[1], ClipTransform[2], ClipTransform[3]);
        transform += mul(Weights[i], current);
    }

    world = mul(transform, world);
}

VertexOutput VS(VertexInput input)
{    
    VertexOutput output;
    
    World = mul(BoneTransforms[BoneIndex], World);
    SetAnimationWorld(World, input);
    
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
	float Light = dot(-LightDirection, normal);
    float3 color = MaterialMaps[0].Sample(Samp, input.Uv).rgb;
    color *= Light;
    return float4(color, 1);
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

/*===========================================================================*/