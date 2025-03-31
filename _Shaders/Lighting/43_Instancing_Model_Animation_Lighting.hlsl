#include "43_WVP.hlsl"
// #include "43_LightingResources.hlsl"
#include "43_AreaLigthing.Fucntions.hlsl"
#include "../00_Animation_Structure.hlsl"

const static int g_BoneCountToFindWeight = 4;
const static int g_MipMapLevel = 0;

cbuffer CB_BoneMatrix : register(Const_VS_BoneMatrix)
{
    matrix OffsetMatrix[MAX_MODEL_TRANSFORM];      // RootToBone Matrix. == Inv(Bone의 Root-Coordinate Transform)
    matrix BoneTransforms[MAX_MODEL_TRANSFORM];    //
}

cbuffer CB_BoneIndex : register(Const_VS_BoneIndex)
{
    uint BaseBoneIndex;
    float3 Padding;
}

cbuffer CB_AnimationBlending : register(Const_VS_AnimationBlending)
{
    BlendingFrame AnimationBlending[MAX_INSTANCE_COUNT];
};

Texture2DArray<float4> ClipsTFMap : register(Texture_VS_KeyFrames);

struct VertexInput
{
    float4 Position  : POSITION;
    float2 Uv        : UV;
    float4 Color     : COLOR;
    float3 Normal    : NORMAL;
    float3 Tangent   : TANGENT;
    float4 Indices   : BLENDINDICES;
    float4 Weight    : BLENDWEIGHTS;
    matrix Transform : INSTANCE;

    // InstanceID : GPU가 자동으로 생성하며, 쉐이더로 전달됩니다. C++ 코드에서 직접 설정할 필요가 없음
    uint InstanceID              : SV_InstanceID;
};

struct VertexOutput
{
    float4 Position      : SV_Position;
    float3 WorldPosition : WPOSITION;
    float2 Uv            : UV;
    float3 Normal        : NORMAL;
    float3 Tangent       : TANGENT;
};

matrix GetClipTransformMatrix(int BoneIndex, int TargetFrame, int ClipIndex);
matrix InterpolateKeyFrameMatrix(in InterploateKeyframeParams Params, int InstanceID = 0);
float4 SetAnimatedBoneToWorldTF_Instancing(inout VertexInput input);

/*======================================================================================================*/

VertexOutput VSMain(VertexInput input)
{    
    matrix TF = input.Transform;
    // matrix TF = WorldTF;
    matrix ModelWorldTF = mul(BoneTransforms[BaseBoneIndex], TF);

    VertexOutput output;
    output.Uv = input.Uv;

    output.Normal = mul(input.Normal, (float3x3)ModelWorldTF);

    output.Position = SetAnimatedBoneToWorldTF_Instancing(input); // Local_Space(Bone Root Space)
    output.Position = mul(output.Position, ModelWorldTF);
    output.Position = mul(output.Position, View_VS);
    output.Position = mul(output.Position, Projection_VS);
    
    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    // float4 NomalMapTexel = MaterialMaps[MATERIAL_TEXTURE_NORMAL].Sample(AnisotropicSampler, input.Uv);
    // input.Normal = NormalMapping(input.Uv, input.Normal, input.Tangent, NomalMapTexel.xyz);

    float4 A = Ambient;
    float4 D = MaterialMaps[MATERIAL_TEXTURE_DIFFUSE].Sample(LinearSampler, input.Uv) * Diffuse;
    float4 S = MaterialMaps[MATERIAL_TEXTURE_SPECULAR].Sample(LinearSampler, input.Uv) * Specular;

    float4 GlobalAmbient = float4(0.2f,0.2f,0.2f,1.f);

    ColorDesc DirectionalLightColor = ApplyGlobalDirectionalLights(
        LightDirection_PS,
        input.WorldPosition,
        ViewInv_PS._41_42_43,
        input.Normal
    );
    ColorDesc PointLightColor = ApplyPointLights(
        input.WorldPosition,
        ViewInv_PS._41_42_43,
        input.Normal
    );
    ColorDesc SpotLightColor = ApplySpotLights(
        input.WorldPosition,
        ViewInv_PS._41_42_43,
        input.Normal
    );

    ColorDesc OutPut;
    OutPut.Ambient  = GlobalAmbient + DirectionalLightColor.Ambient  + PointLightColor.Ambient  + SpotLightColor.Ambient;
    OutPut.Diffuse  = DirectionalLightColor.Diffuse  + PointLightColor.Diffuse  + SpotLightColor.Diffuse;
    OutPut.Specular = DirectionalLightColor.Specular + PointLightColor.Specular + SpotLightColor.Specular;
    OutPut.Ambient  *= A;
    OutPut.Diffuse  *= D;
    OutPut.Specular *= S;
    return OutPut.Ambient + OutPut.Diffuse + OutPut.Specular;
}

/*======================================================================================================*/


float4 SetAnimatedBoneToWorldTF_Instancing(inout VertexInput input)
{
    int   Indices[4] = { input.Indices.x, input.Indices.y, input.Indices.z, input.Indices.w };
    float Weights[4] = { input.Weight.x, input.Weight.y, input.Weight.z, input.Weight.w };

    float4 pos = 0;
    InterploateKeyframeParams ParamsCurrent;
    InterploateKeyframeParams ParamsNext;

    for(int i = 0 ; i < g_BoneCountToFindWeight ; i++)
    {
        int targetBoneIndex = Indices[i];
        float4 VertexPosInBoneSpace = mul(input.Position, OffsetMatrix[targetBoneIndex]);
        
        matrix currentAnim = 0;
        
        ParamsCurrent.TargetFrame = CURR_FRAME;
        ParamsCurrent.BoneIndex = targetBoneIndex;
        ParamsCurrent.Weight = Weights[i];

        currentAnim = InterpolateKeyFrameMatrix(ParamsCurrent, input.InstanceID);

        [flatten]
        if (AnimationBlending[input.InstanceID].Next.Clip >= 0)
        {
            ParamsNext.TargetFrame = NEXT_FRAME;
            ParamsNext.BoneIndex = targetBoneIndex;
            ParamsNext.Weight = Weights[i];
            currentAnim = lerp(currentAnim, InterpolateKeyFrameMatrix(ParamsNext, input.InstanceID), AnimationBlending[input.InstanceID].ElapsedBlendTime);
        }

        pos += mul(VertexPosInBoneSpace, currentAnim);
    }
    return pos;
}

/*======================================================================================================*/

matrix InterpolateKeyFrameMatrix(in InterploateKeyframeParams Params, int InstanceID)
{
    int     AnimationIndex[2]   = {AnimationBlending[InstanceID].Current.Clip,              AnimationBlending[InstanceID].Next.Clip};
    int     CurrentFrame[2]     = {AnimationBlending[InstanceID].Current.CurrentFrame,      AnimationBlending[InstanceID].Next.CurrentFrame};
    int     NextFrame[2]        = {AnimationBlending[InstanceID].Current.NextFrame,         AnimationBlending[InstanceID].Next.NextFrame};
    float   Time[2]             = {frac(AnimationBlending[InstanceID].Current.CurrentTime), frac(AnimationBlending[InstanceID].Next.CurrentTime)};

    const int TargetAnimationIndex = AnimationIndex[Params.TargetFrame];
    const int TargetCurrentFrame = CurrentFrame[Params.TargetFrame];
    const int TargetNextFrame = NextFrame[Params.TargetFrame];
    const int InterpRate = Time[Params.TargetFrame];

    matrix current = GetClipTransformMatrix(Params.BoneIndex, TargetCurrentFrame, TargetAnimationIndex);
    current = mul(current, Params.Weight);

    matrix next = GetClipTransformMatrix(Params.BoneIndex, TargetNextFrame, TargetAnimationIndex);
    next = mul(next, Params.Weight);

    return lerp(current, next, InterpRate);
}

// Load : Texel Data를 Filtering이나 Sampling 없이 읽는다.
// https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/dx-graphics-hlsl-to-load
matrix GetClipTransformMatrix(int BoneIndex, int TargetFrame, int ClipIndex)
{
    float4 ClipTransform[4];

    ClipTransform[0] = ClipsTFMap.Load(int4(BoneIndex * g_BoneCountToFindWeight + 0, TargetFrame, ClipIndex, g_MipMapLevel));
    ClipTransform[1] = ClipsTFMap.Load(int4(BoneIndex * g_BoneCountToFindWeight + 1, TargetFrame, ClipIndex, g_MipMapLevel));
    ClipTransform[2] = ClipsTFMap.Load(int4(BoneIndex * g_BoneCountToFindWeight + 2, TargetFrame, ClipIndex, g_MipMapLevel));
    ClipTransform[3] = ClipsTFMap.Load(int4(BoneIndex * g_BoneCountToFindWeight + 3, TargetFrame, ClipIndex, g_MipMapLevel));
    
    return matrix(ClipTransform[0], ClipTransform[1], ClipTransform[2], ClipTransform[3]);
}