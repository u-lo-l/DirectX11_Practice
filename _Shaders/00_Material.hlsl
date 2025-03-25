#include "Slot.hlsl"

Texture2D MaterialMaps[MAX_MATERIAL_TEXTURE_COUNT] : register(Texture_PS_Texture);

cbuffer CB_Material : register(Const_PS_Material)
{ 
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Emissive;
}