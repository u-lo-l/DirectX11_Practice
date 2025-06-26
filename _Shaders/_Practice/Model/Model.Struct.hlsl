#ifndef __MODEL_STRUCT_HLSL__
# define __MODEL_STRUCT_HLSL__


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

struct DepthOutput
{
    float4 ShadowPosition : SV_Position;
};

struct VertexOutput
{
    float4 Position             : SV_Position;
    float3 WorldPosition        : WPOSITION;
    float4 ProjectorNDCPosition : PROJ_Position;
    float4 ShadowPosition       : SPOSITION;

    float2 Uv            : UV;
    float3 Normal        : NORMAL;
    float3 Tangent       : TANGENT;
};

struct LightingCoefficent
{
    float Ambient;
    float Diffuse;
    float Specular;
};

struct ColorDesc
{
	float4 Ambient;
    float4 Diffuse;
    float4 Specular;
};

struct PointLightDesc
{
	ColorDesc Color;
    float4 Emissive;
    
    float3 Position;
    float Range;
    
    float Intensity;
    float3 Padding;
};


struct SpotLightDesc
{
	ColorDesc Color;
    float4 Emissive;
    
    float3 Position;
    float Range;
    
    float3 Direction;
    float Intensity;

    float Theta;
	float Phi;
    float2 Padding;
};

#endif