#include "ToonShading.Slot.hlsl"

cbuffer CB_World_VS : register(Const_VS_World)
{ 
    matrix WorldTF_VS;
}

cbuffer CB_ViewProjection_VS : register(Const_VS_ViewProjection)
{ 
    matrix View_VS;
    matrix ViewInv_VS;
    matrix Projection_VS;
}

cbuffer CB_ViewProjection : register(Const_PS_ViewInv)
{
    matrix ViewInv_PS;
    float4 Tiling;
}

cbuffer CB_LightDirection_PS : register(Const_PS_LightDirection)
{ 
    float3 LightDirection_PS;
}