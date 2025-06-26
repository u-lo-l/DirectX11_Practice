#include "ToonShading.Lighting.hlsl"

ColorDesc ApplyGlobalDirectionalLights(
    // in ColorDesc MatColor,
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
        WorldPosition
    );
    ColorDesc LightColor = ComputeToonPhongLight(
        LightDirection,
        ViewPosition,
        Normal,
        WorldPosition,
        SunLightColor
    );
    // LightColor.Ambient  += Rim;
    // LightColor.Diffuse  += Rim;
    // LightColor.Specular += Rim;
    return LightColor;
};

ColorDesc ApplyPointLights(
    // in ColorDesc MatColor,
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
            WorldPosition
        );
        ColorDesc Phong = ComputeToonPhongLight(
            WorldPosition - PointLights[i].Position,
            ViewPosition,
            Normal,
            WorldPosition,
            PointLights[i].Color
        );
        float Atten = AttenuationFactor(length(WorldPosition- PointLights[i].Position));
        LightColor.Ambient  += Phong.Ambient  * Atten;
        LightColor.Diffuse  += Phong.Diffuse  * Atten;
        LightColor.Specular += Phong.Specular * Atten;
        // LightColor.Ambient  += Rim;
        // LightColor.Diffuse  += Rim;
        // LightColor.Specular += Rim;
    }

    return LightColor;
}

ColorDesc ApplySpotLights(
    // in ColorDesc MatColor,
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
            WorldPosition
        );
        ColorDesc Phong = ComputeToonPhongLight(
            WorldPosition - SpotLights[i].Position,
            ViewPosition,
            Normal,
            WorldPosition,
            SpotLights[i].Color
        );
        float Atten = AttenuationFactor(length(WorldPosition- SpotLights[i].Position));
        // float Spot = SpotLightFactor(SpotLights[i].Theta, SpotLights[i].Phi, dot(Normal,V));
        float Spot = 1.f;
        LightColor.Ambient  += Phong.Ambient  * Atten * Spot;
        LightColor.Diffuse  += Phong.Diffuse  * Atten * Spot;
        LightColor.Specular += Phong.Specular * Atten * Spot;
        // LightColor.Ambient  += Rim;
        // LightColor.Diffuse  += Rim;
        // LightColor.Specular += Rim;
    }

    return LightColor;
}