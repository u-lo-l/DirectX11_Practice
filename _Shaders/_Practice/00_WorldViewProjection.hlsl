#include "Slot.hlsl"

cbuffer CB_World : register(Const_VS_World)
{ 
    matrix WorldTF;
}

cbuffer CB_ViewProjection : register(Const_VS_ViewProjection)
{ 
    matrix View;
    matrix ViewInv;
    matrix Projection;
}

cbuffer CB_LightDirection : register(Const_PS_LightDirection)
{ 
    float3 LightDirection;
}