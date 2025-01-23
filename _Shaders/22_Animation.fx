#include "00_Context.fx"
#include "00_Material.fx"

#define MAX_MODEL_TRANSFORM 256
cbuffer CB_ModelBones
{
    matrix RootToBoneTF[MAX_MODEL_TRANSFORM]; // RootToBone Matrix. == Inv(Bone의 Root-Coordinate Transform)
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
    uint Clip;           // 몇 번 째 Clip인지
    uint Frame;          // 그 clip에서 몇 번째 Frame인지지

    float2 Padding;
};

cbuffer CB_AnimationFrame
{
    AnimationFrame KeyFrameData;
};

Texture2DArray<float4> ClipsTFMap;

int BoneCountToFindWeight = 4;
int MipMapLevel = 0;

float4 SetAnimatedBoneToWorldTF(VertexInput input)
{
    int   Indices[4] = { input.Indices.x, input.Indices.y, input.Indices.z, input.Indices.w };
    float Weights[4] = { input.Weight.x, input.Weight.y, input.Weight.z, input.Weight.w };
    uint  AnimationIndex = KeyFrameData.Clip;
    uint  CurrentFrame = KeyFrameData.Frame;

    float4 ClipTransform[4];

    matrix current = 0;
    float4 pos = 0;

    // 4개의 Bone에 대해서 Weight를 탐색.
    for(int i = 0 ; i < BoneCountToFindWeight ; i++)
    {
        int targetBoneIndex = Indices[i];
        matrix TargetRootBoneInvMat = RootToBoneTF[targetBoneIndex];
        float4 VertexPosInBoneSpace = mul(input.Position, TargetRootBoneInvMat);

        // Indices[i] : 이 Vertex가 참조 할 Bone. -> Indices[i] * 4 + j : 몇 째 열의 데이터인지
        // CurrentFrame : 몇 째 행의 데이터인지지
        // Clip : 몇 번 째 Animation인지. 지금은 하나만 있다.
        // 0 : MipMap Level : 사용하지 않는다. 0최상위 해상도를 사용한다는 의미.
        ClipTransform[0] = ClipsTFMap.Load(int4(targetBoneIndex * BoneCountToFindWeight + 0, CurrentFrame, AnimationIndex, MipMapLevel));
        ClipTransform[1] = ClipsTFMap.Load(int4(targetBoneIndex * BoneCountToFindWeight + 1, CurrentFrame, AnimationIndex, MipMapLevel));
        ClipTransform[2] = ClipsTFMap.Load(int4(targetBoneIndex * BoneCountToFindWeight + 2, CurrentFrame, AnimationIndex, MipMapLevel));
        ClipTransform[3] = ClipsTFMap.Load(int4(targetBoneIndex * BoneCountToFindWeight + 3, CurrentFrame, AnimationIndex, MipMapLevel));

        //이렇게 접근하면 안 된다. 2차원 배열은 일반적으로 Arr[row][col]이지만 Texture는 Tex[col][row]
        // ClipTransform[0] = ClipsTFMap.Load(int4(CurrentFrame, Indices[i] * 4 + 0, AnimationIndex, 0));

        current = matrix(ClipTransform[0], ClipTransform[1], ClipTransform[2], ClipTransform[3]);
        current = mul(current, Weights[i]);
        pos += mul(VertexPosInBoneSpace, current);
    }
    return pos;
}

VertexOutput VS(VertexInput input)
{    
    VertexOutput output;
    output.Position = SetAnimatedBoneToWorldTF(input); // Local_Space(Bone Root Space)
    output.Position = mul(output.Position, ModelWorldTF);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.Normal = input.Normal;
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