#include "ToonShading.Resources.hlsl"

struct PhongLightingCoefficent
{
    float Ambient;
    float Diffuse;
    float Specular;
};
ColorDesc ComputeToonPhongLight (in float3 LightDirection, in float3 ViewPosition, in float3 Normal, in float3 WorldPosition, in ColorDesc Color);
ColorDesc ComputeToonPhongLight(in float3 LightDirection, in float3 ViewPosition, in float3 Normal, in float3 WorldPosition, in float4 GlobalAmbient, in ColorDesc MaterialColor, in PhongLightingCoefficent Coeff);
float4 ComputeRimLight(float3 LightDirection, float3 ViewPosition, float3 Normal, float3 WorldPosition, float RimWidth);


ColorDesc ComputeToonPhongLight (
    in float3 LightDirection,
    in float3 ViewPosition,
    in float3 Normal,
    in float3 WorldPosition,
    in ColorDesc Color
)
{
    PhongLightingCoefficent Coeff = {1,1,1};
    return ComputeToonPhongLight(
        LightDirection,
        ViewPosition,
        Normal,
        WorldPosition,
        float4(0.1,0.1,0.1,1),
        Color,
        Coeff
    );
}

const static int BandShadingLevel = 7;
ColorDesc ComputeToonPhongLight( // 사실 Blin-Phong임
    in float3 LightDirection,
    in float3 ViewPosition,
    in float3 Normal,
    in float3 WorldPosition,
    in float4 GlobalAmbient,
    in ColorDesc MaterialColor,
    in PhongLightingCoefficent Coeff
)
{
    ColorDesc Phong;
    float3 L = normalize(-LightDirection);
    float3 N = normalize(Normal);
    float NdotL = saturate(dot(L, N));

    Phong.Ambient = Coeff.Ambient * GlobalAmbient * MaterialColor.Ambient;
    
    // float NdotL = ceil(saturate(dot(L, N)) * BandShadingLevel) / BandShadingLevel;
    Phong.Diffuse = Coeff.Diffuse * MaterialColor.Diffuse;
    Phong.Diffuse *= NdotL;
    // Phong.Diffuse *= round(NdotL);
    // Phong.Diffuse *= step(0.2f, NdotL);
    // Phong.Diffuse *= smoothstep(0.2f, 0.25f, NdotL);

    float ns = saturate(MaterialColor.Specular.a) * 128 + 1;
    float3 V = normalize(ViewPosition - WorldPosition); // also called as E for Eye-Vector
    { // Phong
        // float3 R = normalize(reflect(-L, N));
        // float VdotR = saturate(dot(V, R));
        // Phong.Specular = Coeff.Specular * MaterialColor.Specular * pow(VdotR, ns);
    }
    { // Blin Phong
        float3 H = normalize(L + V);
        float NdotH = saturate(dot(H, N));
        Phong.Specular = Coeff.Specular * MaterialColor.Specular * pow(NdotH, ns);
        Phong.Specular = smoothstep(0.2, 0.5, Phong.Specular) * 0.5f;
    }
    { // 

    }

    return Phong;
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
    float a = saturate(1 - 0.25f);
    float b = 1.f;
    float c = saturate(1 - NdotV);
    float Rim = smoothstep(a, b, c);
    Rim = Rim * LdotV;
    // Rim = pow(Rim, RimPower);
    return float4(Rim, Rim, Rim, 1);
}

const static float handed = -1; // for LeftHanded Coordinate
float3 NormalMapping // TODO : Fix Problem
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
    
    float3 NewLocalNormal = normalMapTexelValue * 2.0f - 1.0f; // [0, 1] -> [-1, 1]

    float3 N = normalize(normal); // Z
    float3 T = normalize(tangent - dot(tangent, N) * N); // X
    float3 B = cross(N, T) * handed; // Y
    
    float3x3 TBN =  float3x3(T, B, N);
    float3 WorldNormal = mul(NewLocalNormal, TBN);
    return WorldNormal;
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