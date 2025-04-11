#ifndef __MODEL_HLSL__
# define __MODEL_HLSL__
# include "Model.Slot.hlsl"
# include "Model.Resources.hlsl"
# include "Model.Functions.hlsl"
# include "Model.Struct.hlsl"
// # define USE_ANIMATION 1
# ifdef USE_ANIMATION
#  include "Model.Animation.Functions.hlsl"
# endif


DepthOutput VSShadow(VertexInput Input)
{
    DepthOutput output;
    matrix TF = Input.Transform;
    matrix ModelWorldTF = mul(BoneTransforms[BaseBoneIndex], TF);

#  ifdef USE_ANIMATION
    output.ShadowPosition = SetAnimatedBoneToWorldTF_Instancing(Input); // Local_Space(Bone Root Space)
#  else 
    output.ShadowPosition = Input.Position;
#  endif

    output.ShadowPosition = mul(output.ShadowPosition, ModelWorldTF);
    output.ShadowPosition = mul(output.ShadowPosition, ShadowView);
    output.ShadowPosition = mul(output.ShadowPosition, ShadowProjection);
    return output;
}

float4 PSShadow(DepthOutput input) : SV_Target
{
    float depth = input.ShadowPosition.z / input.ShadowPosition.w;
    return float4 (depth, depth, depth, 1);
}


VertexOutput VSMain(VertexInput Input)
{
    VertexOutput output;

    matrix ModelWorldTF = Input.Transform;
    ModelWorldTF = mul(BoneTransforms[BaseBoneIndex], ModelWorldTF);

    output.Uv = Input.Uv;
    output.Normal = mul(Input.Normal, (float3x3) ModelWorldTF);
    output.Tangent = mul(Input.Tangent, (float3x3) ModelWorldTF);

#  ifdef USE_ANIMATION
    output.Position = SetAnimatedBoneToWorldTF_Instancing(Input);
#  else 
    output.Position = Input.Position;
#  endif

    output.Position = mul(output.Position, ModelWorldTF);
    output.WorldPosition = output.Position.xyz;
    output.Position = mul(output.Position, View_VS);
    output.Position = mul(output.Position, Projection_VS);

    output.ProjectorNDCPosition = mul(float4(output.WorldPosition, 1), View_Projector_VS);
    output.ProjectorNDCPosition = mul(output.ProjectorNDCPosition, Projection_Projector_VS);
    output.ProjectorNDCPosition /= output.ProjectorNDCPosition.w;

    output.ShadowPosition = mul(float4(output.WorldPosition, 1), ShadowView);
    output.ShadowPosition = mul(output.ShadowPosition, ShadowProjection);

    return output;
}

float4 PSMain(VertexOutput input) : SV_Target
{
    ColorDesc Color = ApplyAllLights_PS(input);
    Color = ApplyShadow(Color, input.ShadowPosition, ShadowBias);
    return  Color.Ambient + Color.Diffuse + Color.Specular;
}


#endif