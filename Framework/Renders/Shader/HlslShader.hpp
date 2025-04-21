#pragma once
#include "framework.h"
#include <fstream>
#include "Utilites/String.h"

template <class T>
std::string HlslShader<T>::GetShaderTarget( ShaderType Type )
{
	switch (Type)
	{
	case ShaderType::VertexShader:	 return "vs_5_0";
	case ShaderType::PixelShader:	 return "ps_5_0";
	case ShaderType::GeometryShader: return "gs_5_0";
	case ShaderType::HullShader:     return "hs_5_0";
	case ShaderType::DomainShader:   return "ds_5_0";
	default:
		ASSERT(false, "Unknown Shader Type : ShaderTarget")
		return "";
	}
}

template <class T>
std::string HlslShader<T>::GetEntryPoint( ShaderType Type )
{
	switch (Type)
	{
	case ShaderType::VertexShader:	 return VSEntryPoint.length() < 1 ?  "VSMain" : VSEntryPoint;
	case ShaderType::PixelShader:	 return PSEntryPoint.length() < 1 ?  "PSMain" : PSEntryPoint;
	case ShaderType::GeometryShader: return GSEntryPoint.length() < 1 ?  "GSMain" : GSEntryPoint;
	case ShaderType::HullShader:     return HSEntryPoint.length() < 1 ?  "HSMain" : HSEntryPoint;
	case ShaderType::DomainShader:   return DSEntryPoint.length() < 1 ?  "DSMain" : DSEntryPoint;
	default :
		ASSERT(false, "Unknown Shader Type : ShaderEntryPoint")
		return "";
	}
}


template <class T>
HlslShader<T>::HlslShader
(
	const wstring & ShaderFileName,
	UINT TargetShaderFlag,
	const string & InVSEntryPoint,
	const string & InPSEntryPoint,
	const string & InGSEntryPoint,
	const D3D_SHADER_MACRO * InMacros
)
	: VSEntryPoint(InVSEntryPoint)
	, GSEntryPoint(InGSEntryPoint)
	, PSEntryPoint(InPSEntryPoint)
{
	if (ShaderFileName.empty() == true)
		return ;
	for (UINT i = 0; i < (UINT)SamplerStateType::Max; i++)
	{
		SamplerStates[i].first = nullptr;
		SamplerStates[i].second = 0;
	}
	
	FileName = W_SHADER_PATH + ShaderFileName;
	
	if (TargetShaderFlag & static_cast<UINT>(ShaderType::VertexShader))
		CompileShader(ShaderType::VertexShader, FileName, InMacros);
	if (TargetShaderFlag & static_cast<UINT>(ShaderType::PixelShader))
		CompileShader(ShaderType::PixelShader, FileName, InMacros);
	if (TargetShaderFlag & static_cast<UINT>(ShaderType::GeometryShader))
		CompileShader(ShaderType::GeometryShader, FileName, InMacros);
	if (TargetShaderFlag & static_cast<UINT>(ShaderType::HullShader))
		CompileShader(ShaderType::HullShader, FileName, InMacros);
	if (TargetShaderFlag & static_cast<UINT>(ShaderType::DomainShader))
		CompileShader(ShaderType::DomainShader, FileName, InMacros);
	CHECK(CreateRasterizerState_Solid() >= 0);
	CHECK(CreateBlendState_NoBlend() >= 0);
	CHECK(CreateDepthStencilState_Default() >= 0);
}

template <class T>
HlslShader<T>::~HlslShader()
{
	SAFE_RELEASE(InputLayout);
	
	SAFE_RELEASE(RasterizerState);
	for (UINT i = 0 ; i < (UINT)SamplerStateType::Max ; i++)
	{
		SAFE_RELEASE(SamplerStates[i].first);
	}
	SAFE_RELEASE(BlendState);
	SAFE_RELEASE(DepthStencilState);
	
	SAFE_RELEASE(Prev_RasterizerState);
	SAFE_RELEASE(Prev_SamplerState);
	SAFE_RELEASE(Prev_BlendState);
	SAFE_RELEASE(Prev_DepthStencilState);
	
	SAFE_RELEASE(VertexShader);
	SAFE_RELEASE(PixelShader);
	SAFE_RELEASE(GeometryShader);
}

template <class T>
void HlslShader<T>::SetTopology(D3D_PRIMITIVE_TOPOLOGY InTopology)
{
	Topology = InTopology;
}

// Rasterizer
template <class T>
HRESULT HlslShader<T>::CreateRasterizerState_WireFrame()
{
	D3D11_RASTERIZER_DESC RasterizerDesc;
	RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	RasterizerDesc.CullMode = D3D11_CULL_BACK;
	RasterizerDesc.FrontCounterClockwise = false;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	RasterizerDesc.DepthClipEnable = true;
	RasterizerDesc.ScissorEnable = false;
	RasterizerDesc.MultisampleEnable = false;
	RasterizerDesc.AntialiasedLineEnable = false;
	return this->CreateRasterizerState(&RasterizerDesc);
}

template <class T>
HRESULT HlslShader<T>::CreateRasterizerState_Solid()
{
	D3D11_RASTERIZER_DESC RasterizerDesc;
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_BACK;
	RasterizerDesc.FrontCounterClockwise = false;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	RasterizerDesc.DepthClipEnable = true;
	RasterizerDesc.ScissorEnable = false;
	RasterizerDesc.MultisampleEnable = false;
	RasterizerDesc.AntialiasedLineEnable = false;
	return this->CreateRasterizerState(&RasterizerDesc);
}

template <class T>
HRESULT HlslShader<T>::CreateRasterizerState_Solid_CullFront()
{
	D3D11_RASTERIZER_DESC RasterizerDesc;
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_FRONT;
	RasterizerDesc.FrontCounterClockwise = false;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	RasterizerDesc.DepthClipEnable = true;
	RasterizerDesc.ScissorEnable = false;
	RasterizerDesc.MultisampleEnable = false;
	RasterizerDesc.AntialiasedLineEnable = false;
	return this->CreateRasterizerState(&RasterizerDesc);
}

template <class T>
HRESULT HlslShader<T>::CreateRasterizerState_WireFrame_NoCull()
{
	D3D11_RASTERIZER_DESC RasterizerDesc;
	RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	RasterizerDesc.CullMode = D3D11_CULL_NONE;
	RasterizerDesc.FrontCounterClockwise = false;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	RasterizerDesc.DepthClipEnable = true;
	RasterizerDesc.ScissorEnable = false;
	RasterizerDesc.MultisampleEnable = false;
	RasterizerDesc.AntialiasedLineEnable = false;
	return this->CreateRasterizerState(&RasterizerDesc);
}

template <class T>
HRESULT HlslShader<T>::CreateRasterizerState_Solid_NoCull()
{
	D3D11_RASTERIZER_DESC RasterizerDesc;
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_NONE;
	RasterizerDesc.FrontCounterClockwise = false;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	RasterizerDesc.DepthClipEnable = true;
	RasterizerDesc.ScissorEnable = false;
	RasterizerDesc.MultisampleEnable = false;
	RasterizerDesc.AntialiasedLineEnable = false;
	return this->CreateRasterizerState(&RasterizerDesc);
}

template <class T>
HRESULT HlslShader<T>::CreateRasterizerState_Solid_CW()
{
	D3D11_RASTERIZER_DESC RasterizerDesc;
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_BACK;
	RasterizerDesc.FrontCounterClockwise = true;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	RasterizerDesc.DepthClipEnable = true;
	RasterizerDesc.ScissorEnable = false;
	RasterizerDesc.MultisampleEnable = false;
	RasterizerDesc.AntialiasedLineEnable = false;
	return this->CreateRasterizerState(&RasterizerDesc);
}

// Draw
template <class T>
void HlslShader<T>::Draw( UINT VertexCount, UINT StartVertexLocation )
{
	BeginDraw();
	D3D::Get()->GetDeviceContext()->Draw(VertexCount, StartVertexLocation);
	EndDraw();
}

template <class T>
void HlslShader<T>::DrawIndexed( UINT IndexCount, UINT StartIndexLocation, UINT BaseVertexLocation )
{
	BeginDraw();
	D3D::Get()->GetDeviceContext()->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
	EndDraw();
}

template <class T>
void HlslShader<T>::DrawInstanced( UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation)
{
	BeginDraw();
	D3D::Get()->GetDeviceContext()->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
	EndDraw();
}

template <class T>
void HlslShader<T>::DrawIndexedInstanced
(
	UINT IndexCountPreInstance,
	UINT InstanceCount,
	UINT StartIndexLocation,
	INT BaseVertexLocation,
	UINT StartInstanceLocation
)
{
	BeginDraw();
	D3D::Get()->GetDeviceContext()->DrawIndexedInstanced(
		IndexCountPreInstance,
		InstanceCount,
		StartIndexLocation,
		BaseVertexLocation,
		StartInstanceLocation
	);
	EndDraw();
}

template <class T>
HRESULT HlslShader<T>::CreateRasterizerState( const D3D11_RASTERIZER_DESC * RSDesc )
{
	SAFE_RELEASE(RasterizerState);
	return D3D::Get()->GetDevice()->CreateRasterizerState( RSDesc, &this->RasterizerState );
}

template <class T>
HRESULT HlslShader<T>::CreateSamplerState_Anisotropic(UINT InTargetShade)
{
	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;  // 주소 모드 설정 (기본값: 반복)
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;  // 비교 함수 설정 (기본값: 사용 안함)
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	SamplerDesc.MaxAnisotropy = 4;

	return this->CreateSamplerState(&SamplerDesc, SamplerStateType::Anisotropic, InTargetShade);
}

template <class T>
HRESULT HlslShader<T>::CreateSamplerState_ShadowSampler(UINT InTargetShade)
{
	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	SamplerDesc.MaxAnisotropy = 4;

	return this->CreateSamplerState(&SamplerDesc, SamplerStateType::Shadow, InTargetShade);
}

template <class T>
HRESULT HlslShader<T>::CreateSamplerState_Linear(UINT InTargetShade)
{
	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;  // 주소 모드 설정 (기본값: 반복)
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;  // 비교 함수 설정 (기본값: 사용 안함)
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	return this->CreateSamplerState(&SamplerDesc, SamplerStateType::Linear, InTargetShade);
}

template <class T>
HRESULT HlslShader<T>::CreateSamplerState( const D3D11_SAMPLER_DESC * SampDesc, const SamplerStateType SamplerType, UINT InTargetShader)
{
	SAFE_RELEASE(SamplerStates[(UINT)SamplerType].first);
	SamplerStates[static_cast<UINT>(SamplerType)].second = InTargetShader;
	return D3D::Get()->GetDevice()->CreateSamplerState(SampDesc, &SamplerStates[static_cast<UINT>(SamplerType)].first);
}

template <class T>
HRESULT HlslShader<T>::CreateBlendState_NoBlend()
{
	D3D11_BLEND_DESC BlendDesc = {};
	BlendDesc.AlphaToCoverageEnable = false;
	BlendDesc.IndependentBlendEnable = false;
	BlendDesc.RenderTarget[0].BlendEnable = false;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL ^ D3D11_COLOR_WRITE_ENABLE_ALPHA;
	return CreateBlendState( &BlendDesc );
}

template <class T>
HRESULT HlslShader<T>::CreateBlendState_Opaque()
{
	D3D11_BLEND_DESC BlendDesc = {};
	BlendDesc.AlphaToCoverageEnable = false;
	BlendDesc.IndependentBlendEnable = false;
	
	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	return CreateBlendState( &BlendDesc );
}

template <class T>
HRESULT HlslShader<T>::CreateBlendState_Additive()
{
	D3D11_BLEND_DESC BlendDesc = {};
	BlendDesc.AlphaToCoverageEnable = false;
	BlendDesc.IndependentBlendEnable = false;
	
	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	return CreateBlendState( &BlendDesc );
}

template <class T>
HRESULT HlslShader<T>::CreateBlendState_Multiply()
{
	D3D11_BLEND_DESC BlendDesc = {};
	BlendDesc.AlphaToCoverageEnable = false;
	BlendDesc.IndependentBlendEnable = false;
	
	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	return CreateBlendState( &BlendDesc );
}

template <class T>
HRESULT HlslShader<T>::CreateBlendState_Multiply2X()
{
	D3D11_BLEND_DESC BlendDesc = {};
	BlendDesc.AlphaToCoverageEnable = false;
	BlendDesc.IndependentBlendEnable = false;
	
	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	return CreateBlendState( &BlendDesc );
}


template <class T>
HRESULT HlslShader<T>::CreateBlendState_AlphaBlend()
{
	D3D11_BLEND_DESC BlendDesc = {};
	BlendDesc.AlphaToCoverageEnable = false;
	BlendDesc.IndependentBlendEnable = false;
	
	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	return CreateBlendState( &BlendDesc );
}

template <class T>
HRESULT HlslShader<T>::CreateBlendState_AlphaBlendCoverage()
{
	D3D11_BLEND_DESC BlendDesc = {};
	BlendDesc.AlphaToCoverageEnable = true;
	BlendDesc.IndependentBlendEnable = false;

	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	return CreateBlendState( &BlendDesc );
}

template <class T>
HRESULT HlslShader<T>::CreateBlendState(const D3D11_BLEND_DESC* BlendDesc)
{
	SAFE_RELEASE(BlendState);
	return D3D::Get()->GetDevice()->CreateBlendState(BlendDesc, &BlendState);
}

template <class T>
HRESULT HlslShader<T>::CreateDepthStencilState_Default()
{
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
	DepthStencilDesc.DepthEnable = true;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	DepthStencilDesc.StencilEnable = false;
	return CreateDepthStencilState( &DepthStencilDesc );
}

template <class T>
HRESULT HlslShader<T>::CreateDepthStencilState_NoDepth()
{
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
	DepthStencilDesc.DepthEnable = false;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DepthStencilDesc.StencilEnable = false;
	return CreateDepthStencilState( &DepthStencilDesc );
}

template <class T>
HRESULT HlslShader<T>::CreateDepthStencilState_Particle()
{
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
	DepthStencilDesc.DepthEnable = true;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	DepthStencilDesc.StencilEnable = false;
	return CreateDepthStencilState( &DepthStencilDesc );
}

template <class T>
HRESULT HlslShader<T>::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* DepthStencilDesc)
{
	SAFE_RELEASE(DepthStencilState);
	return D3D::Get()->GetDevice()->CreateDepthStencilState(DepthStencilDesc, &DepthStencilState);
}

template <class T>
void HlslShader<T>::CompileShader
(
	ShaderType Type,
	const wstring & ShaderFileName,
	const D3D_SHADER_MACRO * InMacros
)
{
	ID3DBlob * ErrorBlob = nullptr;
	ID3DBlob * ShaderBlob = nullptr;

	string PreCompiledShaderName = "";
	bool bUsePrecompiledShader = false;
	size_t ExtensionIndex = ShaderFileName.rfind(L".hlsl");
	HRESULT hr = -1;
	ASSERT(ExtensionIndex != string::npos, "ShaderFileName Not Valid");
	PreCompiledShaderName = String::ToString(ShaderFileName);
	PreCompiledShaderName = PreCompiledShaderName.replace(ExtensionIndex, 5, "");
	size_t DirectoryIndex = 0;
	DirectoryIndex = PreCompiledShaderName.rfind("/");
	PreCompiledShaderName = PreCompiledShaderName.replace(ExtensionIndex, 1, "/PreCompiled/");
	// DWORD attrib = GetFileAttributesA(PreCompiledShaderName.c_str());
	// if (attrib == INVALID_FILE_ATTRIBUTES || !(attrib & FILE_ATTRIBUTE_DIRECTORY))
	// {
	// 	CreateDirectoryA(PreCompiledShaderName.c_str(), nullptr);
	// }
	PreCompiledShaderName += GetEntryPoint(Type) + ".cso";
	printf("%s\n", PreCompiledShaderName.c_str());
	std::ifstream file(PreCompiledShaderName, std::ios::binary | std::ios::ate);
	if (file.is_open() == true)
	{
		std::streamsize FileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		hr = D3DCreateBlob(static_cast<SIZE_T>(FileSize), &ShaderBlob);
		file.read((char*)ShaderBlob->GetBufferPointer(), FileSize);
		file.close();
		if (SUCCEEDED(hr))
		{
			bUsePrecompiledShader = true;
		}
	}
	
	// https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/d3dcompile-constants
	int Flag = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
	// Flag |= D3DCOMPILE_OPTIMIZATION_LEVEL1;
	// Flag |= D3DCOMPILE_OPTIMIZATION_LEVEL2;
	Flag |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
	Flag |= D3DCOMPILE_WARNINGS_ARE_ERRORS;

	constexpr int EffectFlag = 0; // Effect FrameWork 쓸 떄만 씀.

	if (bUsePrecompiledShader == false)
	{
		hr = D3DCompileFromFile(
			ShaderFileName.c_str(),
			InMacros,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // HLSL내에서 #include 쓸 수 있게 해줌. custom ID3DInclude도 가능.
			GetEntryPoint(Type).c_str(),
			GetShaderTarget(Type).c_str(),
			Flag,
			EffectFlag,
			&ShaderBlob,
			&ErrorBlob
		);
	}
	if (FAILED(hr) && ErrorBlob != nullptr)
	{
		const char * const ErrMsg = static_cast<char *>(ErrorBlob->GetBufferPointer());
		string ShaderFile = (bUsePrecompiledShader == true) ? PreCompiledShaderName : String::ToString(ShaderFileName);
		SAFE_RELEASE(ErrorBlob);
		ASSERT(false, (String::ToString(ShaderFileName) + " Failed to Compile :\n" + "<" + ErrMsg + ">").c_str())
	}
	if (FAILED(hr) && ErrorBlob == nullptr)
	{
		SAFE_RELEASE(ErrorBlob);
		ASSERT(false, (String::ToString(ShaderFileName) + " Failed to Compile : Maybe No File or Invalid EntryPoint").c_str())
	}
	SAFE_RELEASE(ErrorBlob);
	
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	HRESULT Hr = 0;
	const void * BufferAddr = ShaderBlob->GetBufferPointer();
	const UINT BufferSize = ShaderBlob->GetBufferSize();
	if (bUsePrecompiledShader == false && PreCompiledShaderName.length() > 0)
	{
		std::ofstream outFile(PreCompiledShaderName, std::ios::binary);
		outFile.write((char*)ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize());
		outFile.close();
	}
	if (Type == ShaderType::VertexShader)
	{
		Hr = Device->CreateVertexShader(BufferAddr, BufferSize, nullptr, &VertexShader);
		InitializeInputLayout(ShaderBlob);
	}
	else if (Type == ShaderType::PixelShader)
	{
		Hr = Device->CreatePixelShader(BufferAddr, BufferSize, nullptr, &PixelShader);
	}
	else if (Type == ShaderType::HullShader)
	{
		Hr = Device->CreateHullShader(BufferAddr, BufferSize, nullptr, &HullShader);
	}
	else if (Type == ShaderType::DomainShader)
	{
		Hr = Device->CreateDomainShader(BufferAddr, BufferSize, nullptr, &DomainShader);
	}
	else if (Type == ShaderType::GeometryShader)
	{
		Hr = Device->CreateGeometryShader(BufferAddr, BufferSize, nullptr, &GeometryShader);
	}
	else
	{
		SAFE_RELEASE(ShaderBlob);
		ASSERT(false, "Shader Type Not Supported")
	}
	SAFE_RELEASE(ShaderBlob);
	ASSERT(Hr >= 0, "Failed to create shader")
}

template <class T>
void HlslShader<T>::InitializeInputLayout( ID3DBlob * VertexShaderBlob )
{
	vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutDescs;
	T::CreatInputLayout(InputLayoutDescs);
	const size_t NumElements = InputLayoutDescs.size();
	ASSERT(NumElements > 0, "Input Layout Not Valid")

	const HRESULT Hr = D3D::Get()->GetDevice()->CreateInputLayout(
		InputLayoutDescs.data(),
		NumElements,
		VertexShaderBlob->GetBufferPointer(),
		VertexShaderBlob->GetBufferSize(),
		&InputLayout
	);
	if (FAILED(Hr))
	{
		char* errorMessage = nullptr;

		// HRESULT로부터 에러 메시지를 얻기
		FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			NULL,
			Hr,
			0,
			(LPSTR)&errorMessage,
			0,
			NULL
		);

		std::string errorStr = errorMessage;

		// 메모리 해제
		LocalFree(errorMessage);
	}
	ASSERT((Hr >= 0), "Failed to create input layout")
}

template <class T>
void HlslShader<T>::BeginDraw()
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	DeviceContext->IASetPrimitiveTopology(Topology);
	if (!!InputLayout)
		DeviceContext->IASetInputLayout(InputLayout);
	
	if (!!VertexShader)
		DeviceContext->VSSetShader(VertexShader, nullptr, 0);
	if (!!HullShader)
		DeviceContext->HSSetShader(HullShader, nullptr, 0);
	if (!!DomainShader)
		DeviceContext->DSSetShader(DomainShader, nullptr, 0);
	if (!!GeometryShader)
		DeviceContext->GSSetShader(GeometryShader, nullptr, 0);
	
	if (!!RasterizerState)
	{
		DeviceContext->RSGetState(&Prev_RasterizerState);
		DeviceContext->RSSetState(RasterizerState);
	}
	
	if (!!PixelShader)
		DeviceContext->PSSetShader(PixelShader, nullptr, 0);
	if (!!PixelShader && !!BlendState)
	{
		float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		DeviceContext->OMGetBlendState(&Prev_BlendState, BlendFactor, nullptr);
		DeviceContext->OMSetBlendState(BlendState, BlendFactor, 0xFFFFFFFF);
	}
	if (!!PixelShader && !!DepthStencilState)
	{
		DeviceContext->OMGetDepthStencilState(&Prev_DepthStencilState, nullptr);
		DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);
	}

	for (UINT i = 0 ; i < (UINT)SamplerStateType::Max ; i++)
	{
		ID3D11SamplerState * TargetSampler = (SamplerStates + i)->first;
		UINT TargetShaderType = (SamplerStates + i)->second;
		if (!!TargetSampler)
		{
			if (TargetShaderType | (UINT)ShaderType::VertexShader)
				DeviceContext->VSSetSamplers(i, 1, &TargetSampler);
			if (TargetShaderType | (UINT)ShaderType::HullShader)
				DeviceContext->HSSetSamplers(i, 1, &TargetSampler);
			if (TargetShaderType | (UINT)ShaderType::DomainShader)
				DeviceContext->DSSetSamplers(i, 1, &TargetSampler);
			if (TargetShaderType | (UINT)ShaderType::GeometryShader)
				DeviceContext->GSSetSamplers(i, 1, &TargetSampler);
			if (TargetShaderType | (UINT)ShaderType::PixelShader)
				DeviceContext->PSSetSamplers(i, 1, &TargetSampler);
		}
	}
}

template <class T>
void HlslShader<T>::EndDraw()
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	if (!!Prev_RasterizerState)
	{
		DeviceContext->RSSetState(Prev_RasterizerState);
		SAFE_RELEASE(Prev_RasterizerState);
	}
	// if (!!Prev_SamplerState)
	// {
	// 	DeviceContext->PSSetSamplers(static_cast<UINT>(SamplerSlotNum), 1, &Prev_SamplerState);
	// 	SAFE_RELEASE(Prev_SamplerState);
	// }
	if (!!Prev_BlendState)
	{
		float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		DeviceContext->OMSetBlendState(Prev_BlendState, BlendFactor, 0xFFFFFFFF);
		SAFE_RELEASE(Prev_BlendState);
	}
	if (!!Prev_DepthStencilState)
	{
		DeviceContext->OMSetDepthStencilState(Prev_DepthStencilState,0);
		SAFE_RELEASE(Prev_DepthStencilState);
	}

	
	// 일단 전체 파이프라인에서 HS, DS, GS를 사용하지 않으니 굳이 필요한 코드는 아님.
	// 추후에 다른 RenderPass에서 건드리면 그 때 주석 해제하도록 하자.
	DeviceContext->HSSetShader(nullptr, nullptr, 0);
	DeviceContext->DSSetShader(nullptr, nullptr, 0);
	DeviceContext->GSSetShader(nullptr, nullptr, 0);
}
