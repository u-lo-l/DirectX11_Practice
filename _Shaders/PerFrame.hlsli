#ifndef __PER_FRAME_HLSLI__
#define __PER_FRAME_HLSLI__

cbuffer CB_PerFrame : register(b0)
{
	matrix View;
	matrix Projection;
    float3 CameraWorldPosition;
    float  Padding;
    float4 LightColor;
    float3 LightDirection;
    float  Padding2;
}

#endif