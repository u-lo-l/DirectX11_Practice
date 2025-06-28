#ifndef __TERRAIN_PARAMETER_HLSLI__
#define __TERRAIN_PARAMETER_HLSLI__

struct VS_INPUT
{
	// Vertex
	float4 Position : POSITION;
	float2 UV : UV;

	// Instance
	matrix Transform : INST_MATRIX;
	float2 CellTexCoord : INST_TEXCOORD;
};

struct VS_OUTPUT // HS_INPUT
{
    float4 Position : POSITION;
    float2 UV : UV;

	matrix Transform : INST_MATRIX;
};

struct HS_CONSTANT_OUTPUT
{
    float Edge[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

struct HS_POINT_OUTPUT // DS_INPUT
{
    float4 WorldPosition : WPOSITION;
    float2 UV : UV;
	// matrix Transform : INST_MATRIX;
};

struct DS_OUTPUT // PS_INPUT
{
    float4 Position			: SV_Position;
    float3 WorldPosition	: WPOSITION;
    float2 UV				: UV;

    uint   LOD : LOD;
    float  CameraDistance : DISTANCE;

	float4 DebugColor : COLOR;
};

#endif