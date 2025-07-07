#ifndef __SHADING_FUNC_HLSLI__
#define __SHADING_FUNC_HLSLI__
struct BlinnPhongInput
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float Shininess;

	float4 LightColor;
	float3 LightDirection;

	float3 Normal;
	float3 WorldPosition;
	float3 WorldSpaceCameraPosition;
};

void BlinnPhong
(
	in BlinnPhongInput Input,
	out float4 Ambient,
	out float4 Diffuse,
	out float4 Specular
)
{
	const float3 L = normalize(Input.LightDirection);
	const float3 N = normalize(Input.Normal);
	const float NdotL = saturate(dot(-L, N));

	const float ns = saturate(Input.Shininess) * 128 + 1;
	const float3 R = normalize(reflect(L, N));
	// From Suface To Camera
	// V : E - P
	const float3 V = normalize(Input.WorldSpaceCameraPosition - Input.WorldPosition); // also called as E for Eye-Vector
	const float3 H = normalize(L + V);
	const float NdotH = saturate(dot(H, N));

	const float RimWidth = 1.f;
	const float NdotV = saturate(dot(N, V));
	const float LdotV = saturate(dot(L, V));

	float a = saturate(1 - 0.25f);
	float b = 1.f;
	float c = saturate(1 - NdotV);

	Ambient		= Input.Ambient * Input.Diffuse;
	Diffuse.rgb	= Input.Diffuse.rgb * NdotL;
	Diffuse.a 	= Input.Diffuse.a;
	Specular	= Input.Specular * pow(NdotH, ns);
}

float4 BlinnPhong(in BlinnPhongInput Input)
{
	float4 Result = 0;
	float4 Ambient = 0;
	float4 Diffuse = 0;
	float4 Specular = 0;

	BlinnPhong(Input, Ambient, Diffuse, Specular);

	Result.rgb  += Ambient.rgb * Diffuse.rgb;
	Result.rgba += Diffuse;
	Result.rgb  += Specular.rgb;
	Result.rgb *= Input.LightColor.rgb;
	return Result;
}

struct PBRInput
{
	float4 Albedo;
	float Metalic;
	float Roughness;

	float4 LightColor;
	float3 LightDirection;

	float3 Normal;
	float3 WorldPosition;
	float3 WorldSpaceCameraPosition;
};
struct PBRInputFresnel
{
	float4 Albedo;
	float  Fresnel;

	float4 LightColor;
	float3 LightDirection;

	float3 Normal;
	float3 WorldPosition;
	float3 WorldSpaceCameraPosition;
};
float SchlickApproximation(float Fresnel, float VDotN);

float4 PBR(in PBRInputFresnel Input)
{
	float4 Color;
	const float3 L = normalize(Input.LightDirection);
	const float3 N = normalize(Input.Normal);
	const float NdotL = saturate(dot(-L, N));

	const float3 R = normalize(reflect(L, N));
	const float3 V = normalize(Input.WorldSpaceCameraPosition - Input.WorldPosition); // also called as E for Eye-Vector
	const float VDotN = dot(V, N);

	const float Specular = SchlickApproximation(0, VDotN);

	return Color;
}

float4 PBR(in PBRInput Input)
{
	float4 Color;
	const float3 L = normalize(Input.LightDirection);
	const float3 N = normalize(Input.Normal);
	const float NdotL = saturate(dot(-L, N));

	const float3 R = normalize(reflect(L, N));
	const float3 V = normalize(Input.WorldSpaceCameraPosition - Input.WorldPosition); // also called as E for Eye-Vector
	const float VDotN = dot(V, N);

	const float Specular = SchlickApproximation(0, VDotN);

	return Color;
}


float SchlickApproximation(float Fresnel, float VDotN)
{
    VDotN = max(0, VDotN);
    return (Fresnel + (1 - Fresnel) * pow(1 - VDotN, 5));
}
#endif