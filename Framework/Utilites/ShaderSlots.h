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
};

enum TextureSlot : uint32_t
{
	PS_TextureMap = 0, // Diffuse Texture
	// 1 for Specular Map,
	// 2 for Normal Map.
	VS_KeyFrames = 3,
};

enum SamplerSlot : uint32_t
{
	PS_Default = 0,
};