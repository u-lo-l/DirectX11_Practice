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

float4 BlinnPhong(in BlinnPhongInput Input)
{
	float4 Result = 0;
	float4 Ambient = 0;
	float4 Diffuse = 0;
	float4 Specular = 0;
	float4 Rim = 0;

	const float3 L = normalize(Input.LightDirection);
	const float3 N = normalize(Input.Normal);
	const float NdotL = saturate(dot(-L, N));

	const float ns = saturate(Input.Shininess) * 128 + 1;
	const float3 R = normalize(reflect(L, N));
	// From Suface To Camera
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
	Rim.rgb		= pow(smoothstep(a, b, c) * LdotV, 3);

	Result.rgb  += Ambient.rgb;
	Result.rgba += Diffuse;
	Result.rgb  += Specular.rgb;
	// Result.rgb  += Rim.rgb;
	Result *= Input.LightColor;
	return Result;
}

void PBR(in PBRInput Input ,out float4 Result)
{
	const float3 L = normalize(Input.LightDirection);
	const float3 N = normalize(Input.Normal);
	const float NdotL = saturate(dot(-L, N));

	const float3 R = normalize(reflect(L, N));
	const float3 V = normalize(Input.WorldSpaceCameraPosition - Input.WorldPosition); // also called as E for Eye-Vector
}
#endif