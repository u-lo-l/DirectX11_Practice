// ReSharper disable CppInconsistentNaming
#pragma once

constexpr int VertexSlot = 0;
constexpr int InstancingSlot = 9;

enum ShaderSlot : uint32_t
{
	VS_World = 0,
	VS_ViewProjection = 1,
	VS_BoneMatrix = 2,
	VS_BoneIndex = 3,
	VS_AnimationBlending = 4,

	PS_Material = 5,
	PS_LightDirection = 6,
	PS_ViewInverse = 7,

	GS_World = 9,
	GS_ViewProjection = 10	
};

enum TextureSlot : uint32_t
{
	VS_KeyFrames = 0,
	PS_Billboard = 0,
	PS_SkyBox = 0,
	PS_TextureMap = 1, // Diffuse Texture
	// 1 for Specular Map,
	// 2 for Normal Map.
};

enum SamplerSlot : uint32_t
{
	PS_Linear = 0,	
	PS_Anisotropic = 1,	
};