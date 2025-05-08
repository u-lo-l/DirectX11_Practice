#ifndef __OCEAN_HLSL__
#define __OCEAN_HLSL__

# define DOMAIN "quad"
# define HS_PARTITION "integer"
// # define HS_PARTITION "fractional_even"
// # define HS_PARTITION "fractional_odd"
// # define HS_PARTITION "pow2"
# define HS_INPUT_PATCH_SIZE 4
# define HS_OUTPUT_PATCH_SIZE 4

const static float MinTessFactor = 1;
const static float MaxTessFactor = 64;
const static float RDRatio = 4;

cbuffer CB_WVP : register(b0)  // VS DS HS PS
{
    matrix World : packoffset(c0);
    matrix View : packoffset(c4);
    matrix Projection : packoffset(c8);
}

cbuffer CB_HeightScaler : register(b1)
{
    float3 CameraPosition;
    float HeightScaler = 30.f;

    float2 LODRange;
    float2 TexelSize;

    float ScreenDistance;
    float ScreenDiagonal;
}

cbuffer CB_DirectionalLight : register(b2) // PS
{
    float3 LightDirection;
}

SamplerState LinearSampler : register(s0); // VS DS PS
SamplerState AnisotropicSampler : register(s1); // VS DS PS
Texture2D HeightMap : register(t0);        // VS DS PS


#endif