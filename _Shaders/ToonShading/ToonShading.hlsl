#include "ToonShading.WVP.hlsl"
#include "ToonShading.LightingFunctions.hlsl"

cbuffer CB_BoneMatrix : register(Const_VS_BoneMatrix)
{
    matrix OffsetMatrix[MAX_MODEL_TRANSFORM];   // (Bone의 Root-Coordinate Transform)의 역행렬
    matrix BoneTransforms[MAX_MODEL_TRANSFORM]; //
}

cbuffer CB_BoneIndex : register(Const_VS_BoneIndex)
{
    uint BaseBoneIndex;
    float3 Padding;
}

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
    uint InstanceID  : SV_InstanceID;
};

struct VertexOutput
{
    float4 Position      : SV_Position;
    float3 WorldPosition : WPOSITION;
    float2 Uv            : UV;
    float3 Normal        : NORMAL;
    float3 Tangent       : TANGENT;
    float4 ProjectorNDCPosition : PROJ_Position;
};

// struct MeshOutput
// {
//     float4 Position : SV_Position;
//     float3 oPosition : Position1; //Origin Position
//     float3 wPosition : Position2; //World Position
//     float4 wvpPosition : Position3; //WVP Position
//     float4 wvpPosition_Sub : Position4; //Sub WVP Position
    
//     float2 Uv : Uv;
//     float3 Normal : Normal;
//     float3 Tangent : Tangent;
// };
/*======================================================================================================*/

VertexOutput VSMain(VertexInput input)
{    
    matrix TF = input.Transform;
    matrix ModelWorldTF = mul(BoneTransforms[BaseBoneIndex], TF);

    VertexOutput output;
    output.Uv = input.Uv;
    output.Normal = normalize(mul(input.Normal, (float3x3)ModelWorldTF));
    output.Tangent = normalize(mul(input.Tangent, (float3x3)ModelWorldTF));
    
    output.Position = mul(input.Position, ModelWorldTF);
    output.WorldPosition = output.Position.xyz;

    output.Position = mul(output.Position, View_VS);
    output.Position = mul(output.Position, Projection_VS);
    
    output.ProjectorNDCPosition = mul(input.Position, View_Projector_VS);
    output.ProjectorNDCPosition = mul(output.ProjectorNDCPosition, Projection_Projector_VS);
    output.ProjectorNDCPosition /= output.ProjectorNDCPosition.w;
    return output;
}

float4 ApplyAllLights_PS(VertexOutput input);
float4 ApplyProjector_PS(in float4 Color, in float4 wvp);
float4 PSMain(VertexOutput input) : SV_Target
{
    float4 Color = ApplyAllLights_PS(input);
    Color = ApplyProjector_PS(Color, input.ProjectorNDCPosition);

    return Color;
}

float4 ApplyAllLights_PS(VertexOutput input)
{
    input.Uv.x *= Tiling.x;
    input.Uv.y *= Tiling.y;

    float3 OriginalNormal = input.Normal;

    float4 NomalMapTexel = MaterialMaps[MATERIAL_TEXTURE_NORMAL].Sample(LinearSampler , input.Uv);
    float3 NewNormal = NormalMapping(input.Uv, input.Normal, input.Tangent, NomalMapTexel.xyz);
    
    float4 A = Ambient;
    float4 D = MaterialMaps[MATERIAL_TEXTURE_DIFFUSE].Sample(LinearSampler, input.Uv) * Diffuse;
    float4 S = MaterialMaps[MATERIAL_TEXTURE_SPECULAR].Sample(LinearSampler, input.Uv) * Specular;

    float4 GlobalAmbient = float4(0.2f,0.2f,0.2f,1.f);

    ColorDesc DirectionalLightColor = ApplyGlobalDirectionalLights(
        LightDirection_PS,
        input.WorldPosition,
        ViewInv_PS._41_42_43,
        NewNormal
    );
    ColorDesc PointLightColor = ApplyPointLights(
        input.WorldPosition,
        ViewInv_PS._41_42_43,
        NewNormal
    );
    ColorDesc SpotLightColor = ApplySpotLights(
        input.WorldPosition,
        ViewInv_PS._41_42_43,
        NewNormal
    );

    ColorDesc OutPut;
    OutPut.Ambient  = GlobalAmbient + DirectionalLightColor.Ambient + PointLightColor.Ambient  + SpotLightColor.Ambient;
    OutPut.Diffuse  =                 DirectionalLightColor.Diffuse + PointLightColor.Diffuse  + SpotLightColor.Diffuse;
    OutPut.Specular =                 DirectionalLightColor.Specular + PointLightColor.Specular + SpotLightColor.Specular;
    
    OutPut.Ambient  *= A;
    // OutPut.Diffuse  *= D;
    OutPut.Specular *= S;
    return OutPut.Ambient + OutPut.Diffuse + OutPut.Specular;
}

float4 ApplyProjector_PS(in float4 Color, in float4 ProjectorNDCPosition)
{
    float3 uvw = 0;
    
    uvw.x =  ProjectorNDCPosition.x  * 0.5f + 0.5f;
    uvw.y = -ProjectorNDCPosition.y  * 0.5f + 0.5f;
    uvw.z =  ProjectorNDCPosition.z ;
    
    [flatten]
    if (saturate(uvw.x) == uvw.x && saturate(uvw.y) == uvw.y && saturate(uvw.z) == uvw.z)
    {
        Color = lerp(Color, float4(1, 0, 0, 1), uvw.z);
        // float4 DecalImage = ProjectorTexture.Sample(LinearSampler, uvw.xy);
        
        // DecalImage.rgb *= ProjectorData.Color;
        // Color = lerp(Color, DecalImage, DecalImage.a);
    }
    
    return Color;
}