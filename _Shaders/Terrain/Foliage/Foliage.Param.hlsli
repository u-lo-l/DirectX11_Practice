#ifndef __FOLIAGE_PARAM_HLSLI__
#define __FOLIAGE_PARAM_HLSLI__

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 Scale    : SCALE;
	uint   MapIndex : MAPINDEX;
	float  Random   : RANDOM;	//between 0, 1;
};

struct VS_OUTPUT    // GS_INPUT
{
	float4 WPosition : WPOSITION;
	float2 Scale    : SCALE;
	float2 UV       : UV;
	uint   MapIndex : MAPINDEX;
	float  Random   : RANDOM;	//between 0, 1;
};

struct GS_OUTPUT    // PS_INPUT
{
	float4 Position : SV_Position;
	float3 WPosition : WPOSITION;

	float2 UV       : UV;
	float2 TexCord  : TEXCORD;
	uint   MapIndex : MAPINDEX;
	float  MipLevel : MIP;
};

#endif