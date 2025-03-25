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
	VS_KeyFrames = 0,
	PS_Texture = 5,
};