// #ifndef __SHADOWMAP_HLSL__
// # define __SHADOWMAP_HLSL__

// cbuffer CB_ShadowData_VS : register (b0)
// {
//     matrix Shadow_View;
//     matrix Shadow_Projection;
// }

// struct VertexInput
// {
//     float4 Position  : POSITION;
//     matrix Transform : INSTANCE;

//     // InstanceID : GPU가 자동으로 생성하며, 쉐이더로 전달됩니다. C++ 코드에서 직접 설정할 필요가 없음
//     uint InstanceID  : SV_InstanceID;
// };

// struct VertexOutput
// {
//     float4 Position         : SV_POSITION;
//     float4 ShadowPosition   : SPOSITION;
// };

// VertexOutput VSShadow(VertexInput input)
// {
//     matrix TF = input.Transform;
//     matrix ModelWorldTF = mul(BoneTransforms[BaseBoneIndex], TF);

//     VertexOutput output;
    
//     output.Position = mul(input.Position, ModelWorldTF);
//     output.ShadowPosition = mul(output.Position, Shadow_View);
//     output.ShadowPosition = mul(output.ShadowPosition, Shadow_Projection);
//     return output;
// }

// float4 PSShadow(VertexOutput input) : SV_Target
// {
//     float depth = input.ShadowPosition.z / input.ShadowPosition.w;
//     return float4 (depth, depth, depth, 1);
// }
// #endif