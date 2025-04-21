#ifndef __MODEL_FUNCTIONS_HLSL__
# define __MODEL_FUNCTIONS_HLSL__
# include "Model.Struct.hlsl"
# include "Model.Resources.hlsl"

float3 NormalMapping
(
    float2 uv,
    float3 normal,
    float3 tangent,
    float3 normalMapTexelValue
)
{
    [flatten]
    if (any(normalMapTexelValue.rgb) == false) // 잘못된 TBN-coord가 생성됨.
        return normal;
    
    float3 NewLocalNormal = normalMapTexelValue * 2.0f - 1.0f; //-1.0f ~ +1.0f
    
    float3 N = normalize(normal); // z
    float3 T = -normalize(tangent - dot(tangent, N) * N); // x
    float3 B = cross(N, T); // y

    return normalize(mul(NewLocalNormal, float3x3(T, B, N)));
}

ColorDesc ComputeLight
(
    in float3 LightDirection,
    in float3 ViewPosition,
    in float3 Normal,
    in float3 WorldPosition,
    in float4 GlobalAmbient,
    in ColorDesc MaterialColor,
    in LightingCoefficent Coeff
)
{
    ColorDesc Phong;
    float3 L = normalize(-LightDirection);
    float3 N = normalize(Normal);
    Phong.Ambient = Coeff.Ambient * GlobalAmbient * MaterialColor.Ambient;

    float NdotL = saturate(dot(L, N));
    Phong.Diffuse = Coeff.Diffuse * MaterialColor.Diffuse * NdotL;

    float ns = saturate(MaterialColor.Specular.a) * 128 + 1;
    float3 R = normalize(reflect(-L, N));
    float3 V = normalize(ViewPosition - WorldPosition); // also called as E for Eye-Vector
    float VdotR = saturate(dot(V, R));
    Phong.Specular = Coeff.Specular * MaterialColor.Specular * pow(VdotR, ns);

    return Phong;
}

ColorDesc ComputeLight(
    in float3 LightDirection,
    in float3 ViewPosition,
    in float3 Normal,
    in float3 WorldPosition,
    in ColorDesc Color
)
{
    LightingCoefficent Coeff = {1,1,1};
    return ComputeLight(
        LightDirection,
        ViewPosition,
        Normal,
        WorldPosition,
        float4(0,0,0,0),
        Color,
        Coeff
    );
}

float4 ComputeRimLight(
    float3 LightDirection,
    float3 ViewPosition,
    float3 Normal,
    float3 WorldPosition
)
{
    [flatten]
    if (RimWidth == 0)
        return float4(0,0,0,1);
    float3 L = normalize(LightDirection);
    float3 N = normalize(Normal);
    float3 V = normalize(ViewPosition - WorldPosition);

    float NdotV = saturate(dot(N, V));
    float LdotV = saturate(dot(L, V));
    float a = saturate(1 - RimWidth);
    float b = 1.f;
    float c = saturate(1 - NdotV);
    float Rim = smoothstep(a, b, c);
    Rim = Rim * LdotV;
    // Rim = pow(Rim, RimPower);
    return float4(Rim, Rim, Rim, 1);
}

const static float A1 = 1.f;
const static float A2 = 0.1f;
const static float A3 = 0.01f;
float AttenuationFactor(float Distance)
{
    return 1 / (A1 + A2 * Distance + A3 * pow(Distance, 2));
}

const static float Falloff = 1.;
float SpotLightFactor(float Theta, float Phi, float NdotV)
{
    [flatten] if (cos(Theta/2) < NdotV) return 0;
    [flatten] if (NdotV < cos(Phi/2))   return 1;

    float Factor = (NdotV - cos(Phi/2)) / (cos(Theta) - cos(Phi));
    return pow(Factor, Falloff);
}


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
        WorldPosition
    );
    ColorDesc LightColor = ComputeLight(
        LightDirection,
        ViewPosition,
        Normal,
        WorldPosition,
        SunLightColor
    );
    LightColor.Ambient  += Rim;
    LightColor.Diffuse  += Rim;
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
            WorldPosition
        );
        ColorDesc Phong = ComputeLight(
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
        ColorDesc Phong = ComputeLight(
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
#ifdef USE_PROJECTOR
float4 ApplyProjector_PS(in float4 Color, in float4 ProjectorNDCPosition)
{
    float3 uvw = 0;
    
    uvw.x =  ProjectorNDCPosition.x  * 0.5f + 0.5f;
    uvw.y = -ProjectorNDCPosition.y  * 0.5f + 0.5f;
    uvw.z =  ProjectorNDCPosition.z ;
    
    [flatten]
    if (saturate(uvw.x) == uvw.x && saturate(uvw.y) == uvw.y && saturate(uvw.z) == uvw.z)
    {
        float4 DecalColor = ProjectorTexture.Sample(LinearSampler, uvw.xy);
        return lerp(Color, DecalColor, DecalColor.a);
    }
    return Color;
}
#endif

ColorDesc ApplyAllLights_PS(VertexOutput input)
{
    input.Uv.x *= Tiling.x;
    input.Uv.y *= Tiling.y;

    float3 OriginalNormal = input.Normal;

    float4 NomalMapTexel = MaterialMaps[MATERIAL_TEXTURE_NORMAL].Sample(LinearSampler , input.Uv);
    float3 NewNormal = NormalMapping(input.Uv, input.Normal, input.Tangent, NomalMapTexel.xyz);
    
    ColorDesc MatColor;
    MatColor.Ambient = Ambient;
    MatColor.Diffuse = MaterialMaps[MATERIAL_TEXTURE_DIFFUSE].Sample(LinearSampler, input.Uv) * Diffuse;
    MatColor.Specular = MaterialMaps[MATERIAL_TEXTURE_SPECULAR].Sample(LinearSampler, input.Uv) * Specular;
#ifdef USE_PROJECTOR
    MatColor.Diffuse = ApplyProjector_PS(MatColor.Diffuse, input.ProjectorNDCPosition);
#endif

    float4 GlobalAmbient = float4(0.2f,0.2f,0.2f,1.f);

    ColorDesc DirectionalLightColor = ApplyGlobalDirectionalLights(
        LightDirection_PS,
        input.WorldPosition,
        ViewInv_PS._41_42_43,
        NewNormal
    );
    ColorDesc PointLightColor = ApplyPointLights(
        input.WorldPosition,
        ViewInv_PS._41_42_43,
        NewNormal
    );
    ColorDesc SpotLightColor = ApplySpotLights(
        input.WorldPosition,
        ViewInv_PS._41_42_43,
        NewNormal
    );

    ColorDesc OutPut;
    OutPut.Ambient  = GlobalAmbient + DirectionalLightColor.Ambient  + PointLightColor.Ambient  + SpotLightColor.Ambient;
    OutPut.Diffuse  =                 DirectionalLightColor.Diffuse  + PointLightColor.Diffuse  + SpotLightColor.Diffuse;
    OutPut.Specular =                 DirectionalLightColor.Specular + PointLightColor.Specular + SpotLightColor.Specular;
    

    OutPut.Ambient  *= MatColor.Ambient;
    OutPut.Diffuse  *= MatColor.Diffuse;
    OutPut.Specular *= MatColor.Specular;
    return OutPut;
}

ColorDesc ApplyShadow(in ColorDesc Color, float4 ShadowPosition, float Bias)
{
    float depth = 0;
    float factor = 0;

    ShadowPosition /= ShadowPosition.w;
    [flatten]
    if (ShadowPosition.x < -1.0f || ShadowPosition.x > +1.0f ||
        ShadowPosition.y < -1.0f || ShadowPosition.y > +1.0f ||
        ShadowPosition.z < +0.0f || ShadowPosition.z > +1.0f)
    {
        return Color;
    }

    ShadowPosition.x =  ShadowPosition.x * 0.5f + 0.5f;
    ShadowPosition.y = -ShadowPosition.y * 0.5f + 0.5f;
    ShadowPosition.z =  ShadowPosition.z - Bias;
    
    { // No - PCF
        depth = ShadowMap.Sample(AnisotropicSampler, ShadowPosition.xy).r;
        factor = (float) (depth >= ShadowPosition.z);
    }
    { // PCF
        // depth = input.ShadowPosition.z;
        // factor = ShadowMap.SampleCmpLevelZero(ShadowSampler, input.ShadowPosition.xy, depth).r;
    }

    Color.Diffuse *= saturate(factor + 0.2f);
    Color.Specular *= saturate(factor + 0.2f);

    return Color;
}
#endif