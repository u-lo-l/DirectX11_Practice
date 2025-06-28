#pragma once

enum class ShaderType : UINT
{
	None			= 0,
	PixelShader		= 1 << 0,
	VertexShader	= 1 << 1,
	GeometryShader	= 1 << 2,
	HullShader		= 1 << 3,
	DomainShader	= 1 << 4,
	ComputeShader	= 1 << 5,
	
	VP = PixelShader | VertexShader,
	VG = VertexShader | GeometryShader,
	VGP = VertexShader | GeometryShader | PixelShader,
	HD = HullShader | DomainShader,
	DH = HullShader | DomainShader,
	VD = VertexShader | DomainShader,
	VDP = VertexShader | DomainShader | PixelShader,
	VHDP = VertexShader | HullShader | DomainShader | PixelShader,
	ALL = PixelShader | VertexShader | GeometryShader | HullShader | DomainShader,
};

inline bool operator&(ShaderType Lhs, ShaderType Rhs)
{
	return static_cast<UINT>(Lhs) & static_cast<UINT>(Rhs);
}

struct SamplerStateDesc
{
	// TODO : shared_ptr
	ID3D11SamplerState * SamplerState = nullptr;
	ShaderType TargetShader = ShaderType::PixelShader;
};