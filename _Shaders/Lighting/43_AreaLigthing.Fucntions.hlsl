#include "43_Lighting.hlsl"

ColorDesc ApplyGlobalDirectionalLights(
    in float3 LightDirection,
    in float3 WorldPosition,
    in float3 ViewPosition,
    in float3 Normal
){
    ColorDesc SunLightColor = {
        float4(1,1,1,1),
        float4(1,1,1,1),
        float4(1,1,1,1)
    };
    float4 Rim = ComputeRimLight(
        LightDirection,
        ViewPosition,
        Normal,
        WorldPosition,
        0.3f
    );
    ColorDesc LightColor = ComputePhongLight(
        LightDirection,
        ViewPosition,
        Normal,
        WorldPosition,
        SunLightColor
    );
    LightColor.Ambient += Rim;
    LightColor.Diffuse += Rim;
    LightColor.Specular += Rim;
    return LightColor;
};

ColorDesc ApplyPointLights(
    in float3 WorldPosition,
    in float3 ViewPosition,
    in float3 Normal
)
{
    ColorDesc LightColor = {
        float4(0,0,0,0),
        float4(0,0,0,0),
        float4(0,0,0,0)
    };
    for (uint i = 0 ; i < PointLightNum; i++)
    {
        float3 LightDir = PointLights[i].Position - WorldPosition;
        float Distance = length(LightDir);
        LightDir = normalize(LightDir);

        // [flatten]
        // if (Distance > PointLights[i].Range)
        //     continue;
        float4 Rim = ComputeRimLight(
            LightDir,
            ViewPosition,
            Normal,
            WorldPosition,
            0.3f
        );
        ColorDesc Phong = ComputePhongLight(
            WorldPosition - PointLights[i].Position,
            ViewPosition,
            Normal,
            WorldPosition,
            PointLights[i].Color
        );
        float Atten = AttenuationFactor(length(WorldPosition- PointLights[i].Position));
        LightColor.Ambient += Phong.Ambient * Atten + Rim;
        LightColor.Diffuse += Phong.Diffuse * Atten + Rim;
        LightColor.Specular += Phong.Specular * Atten + Rim;
    }

    return LightColor;
}

ColorDesc ApplySpotLights(
    in float3 WorldPosition,
    in float3 ViewPosition,
    in float3 Normal
)
{
    ColorDesc LightColor = {
        float4(0,0,0,0),
        float4(0,0,0,0),
        float4(0,0,0,0)
    };
    for (uint i = 0 ; i < SpotLightNum; i++)
    {
        float3 LightDir = SpotLights[i].Position - WorldPosition;
        float Distance = length(LightDir);
        LightDir = normalize(LightDir);
        float3 V = normalize(ViewPosition - ViewPosition);

        // [flatten]
        // if (Distance > SpotLights[i].Range)
        //     continue;
        float4 Rim = ComputeRimLight(
            LightDir,
            ViewPosition,
            Normal,
            WorldPosition,
            0.3f
        );
        ColorDesc Phong = ComputePhongLight(
            WorldPosition - SpotLights[i].Position,
            ViewPosition,
            Normal,
            WorldPosition,
            SpotLights[i].Color
        );
        float Atten = AttenuationFactor(length(WorldPosition- SpotLights[i].Position));
        // float Spot = SpotLightFactor(SpotLights[i].Theta, SpotLights[i].Phi, dot(Normal,V));
        // float Atten = 1.f;
        float Spot = 1.f;
        LightColor.Ambient += Phong.Ambient * Atten * Spot + Rim;
        LightColor.Diffuse += Phong.Diffuse * Atten * Spot + Rim;
        LightColor.Specular += Phong.Specular * Atten * Spot + Rim;
    }

    return LightColor;
}