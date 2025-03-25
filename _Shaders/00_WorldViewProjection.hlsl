#include "Slot.hlsl"

cbuffer CB_World : register(Const_VS_World)
{ 
    matrix WorldTF;
}

cbuffer CB_ViewProjection : register(Const_VS_VieProjection)
{ 
    matrix View;
    matrix Projection;
    float3 LightDirection;
}