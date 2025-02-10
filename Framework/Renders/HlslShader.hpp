#pragma once
#include "framework.h"
#include "HlslShader.h"

template <class T, enable_if_t<is_base_of_v<ShaderInputType, T>, int> E0>
constexpr string HlslShader<T, E0>::GetShaderTarget( D3D11_SHADER_VERSION_TYPE Type )
{
	switch (Type)
	{
	case D3D11_SHVER_VERTEX_SHADER:		return "vs_5_0";
	case D3D11_SHVER_PIXEL_SHADER:		return "ps_5_0";
	case D3D11_SHVER_COMPUTE_SHADER:	return "cs_5_0";
	default:
		ASSERT(false, "Unknown Shader Type")
		return "";
	}
}

template <class T, enable_if_t<is_base_of_v<ShaderInputType, T>, int> E0>
constexpr string HlslShader<T, E0>::GetEntryPoint( D3D11_SHADER_VERSION_TYPE Type )
{
	switch (Type)
	{
	case D3D11_SHVER_VERTEX_SHADER:		return "VSMain";
	case D3D11_SHVER_PIXEL_SHADER:		return "PSMain";
	case D3D11_SHVER_COMPUTE_SHADER:	return "CSMain";
	default :
		ASSERT(false, "Unknown Shader Type")
		return "";
	}
}

template <class T, enable_if_t<is_base_of_v<ShaderInputType, T>, int> E0>
HlslShader<T, E0>::HlslShader(const wstring & VsFileName, const wstring & PsFileName, const wstring & CsFileName)
	: DeviceContext(D3D::Get()->GetDeviceContext())
	, VertexShader(nullptr)
	, PixelShader(nullptr)
	, ComputeShader(nullptr)
	, InputLayout(nullptr)
{
	if (VsFileName.empty() == false)
		CompileShader(D3D11_SHVER_VERTEX_SHADER, VsFileName);
	if (PsFileName.empty() == false)
		CompileShader(D3D11_SHVER_PIXEL_SHADER, PsFileName);
	if (CsFileName.empty() == false)
		CompileShader(D3D11_SHVER_COMPUTE_SHADER, CsFileName);
}

template <class T, enable_if_t<is_base_of_v<ShaderInputType, T>, int> E0>
HlslShader<T, E0>::~HlslShader()
{
	SAFE_RELEASE(VertexShader);
	SAFE_RELEASE(PixelShader);
	SAFE_RELEASE(ComputeShader);
	SAFE_RELEASE(InputLayout);
}

template <class T, enable_if_t<is_base_of_v<ShaderInputType, T>, int> E0>
void HlslShader<T, E0>::Draw( UINT VertexCount, UINT StartVertexLocation ) const
{
	BeginDraw();
	DeviceContext->Draw(VertexCount, StartVertexLocation);
	EndDraw();
}

template <class T, enable_if_t<is_base_of_v<ShaderInputType, T>, int> E0>
void HlslShader<T, E0>::DrawIndexed( UINT IndexCount, UINT StartIndexLocation, UINT BaseVertexLocation ) const
{
	BeginDraw();
	DeviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
	EndDraw();
}

template <class T, enable_if_t<is_base_of_v<ShaderInputType, T>, int> E0>
void HlslShader<T, E0>::DrawInstanced( UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation) const
{
	BeginDraw();
	DeviceContext->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
	EndDraw();
}

template <class T, enable_if_t<is_base_of_v<ShaderInputType, T>, int> E0>
void HlslShader<T, E0>::DrawIndexedInstanced
(
	UINT IndexCountPreInstance,
	UINT InstanceCount,
	UINT StartIndexLocation,
	INT BaseVertexLocation,
	UINT StartInstanceLocation
) const
{
	BeginDraw();
	DeviceContext->DrawIndexedInstanced(
		IndexCountPreInstance,
		InstanceCount,
		StartIndexLocation,
		BaseVertexLocation,
		StartInstanceLocation
	);
	EndDraw();
}

template <class T, enable_if_t<is_base_of_v<ShaderInputType, T>, int> E0>
void HlslShader<T, E0>::Dispatch( UINT X, UINT Y, UINT Z ) const
{
	DeviceContext->Dispatch(X, Y, Z);

	// Best-Practice임. 하지만 근거는 이해하지 못함.
	// https://github.com/walbourn/directx-sdk-samples/blob/main/BasicCompute11/BasicCompute11.cpp
	ID3D11ShaderResourceView * NullSRV = nullptr;
	DeviceContext->CSSetShaderResources(0, 1, &NullSRV);
	ID3D11UnorderedAccessView * NullUAV = nullptr;
	DeviceContext->CSSetUnorderedAccessViews(0,1, &NullUAV, nullptr);
	DeviceContext->CSSetShader(nullptr, nullptr, 0);
}

template <class T, enable_if_t<is_base_of_v<ShaderInputType, T>, int> E0>
void HlslShader<T, E0>::CompileShader( D3D11_SHADER_VERSION_TYPE Type, const wstring & ShaderFileName )
{
	ID3DBlob * ErrorBlob = nullptr;
	ID3DBlob * ShaderBlob = nullptr;

	// https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/d3dcompile-constants
	int Flag = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
	// flag |= D3DCOMPILE_OPTIMIZATION_LEVEL1;
	// flag |= D3DCOMPILE_OPTIMIZATION_LEVEL2;
	// flag |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
	Flag |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
	
	constexpr int EffectFlag = 0; // Effect FrameWork 쓸 떄만 씀.
	
	HRESULT hr = D3DCompileFromFile(
		ShaderFileName.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		GetEntryPoint(Type).c_str(),
		GetShaderTarget(Type).c_str(),
		Flag,
		EffectFlag,
		&ShaderBlob,
		&ErrorBlob
	);
	if (FAILED(hr) && ErrorBlob != nullptr)
		ASSERT(false, (String::ToString(ShaderFileName) + ::GetD3D11ReturnMessage(hr)).c_str());
	if (FAILED(hr) && ErrorBlob == nullptr)
		ASSERT(false, (String::ToString(ShaderFileName) + " Failed to Compile").c_str())
	SAFE_RELEASE(ErrorBlob);
	
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	HRESULT Hr = 0;
	const void * BufferAddr = ShaderBlob->GetBufferPointer();
	const UINT BufferSize = ShaderBlob->GetBufferSize();
	if (Type == D3D11_SHVER_VERTEX_SHADER)
	{
		Hr = Device->CreateVertexShader(BufferAddr, BufferSize, nullptr, &VertexShader);
		InitializeInputLayout(ShaderBlob);
	}
	else if (Type == D3D11_SHVER_PIXEL_SHADER)
	{
		Hr = Device->CreatePixelShader(BufferAddr, BufferSize, nullptr, &PixelShader);
	}
	else if (Type == D3D11_SHVER_COMPUTE_SHADER)
	{
		Hr = Device->CreateComputeShader(BufferAddr, BufferSize, nullptr, &ComputeShader);
	}
	else
	{
		SAFE_RELEASE(ShaderBlob);
		ASSERT(false, "Shader Type Not Supported")
	}
	SAFE_RELEASE(ShaderBlob);
	ASSERT(Hr >= 0, "Failed to create shader")
}

template <class T, enable_if_t<is_base_of_v<ShaderInputType, T>, int> E0>
void HlslShader<T, E0>::InitializeInputLayout( ID3DBlob * VertexShaderBlob )
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
	ASSERT((Hr >= 0), "Failed to create input layout")
}

template <class T, enable_if_t<is_base_of_v<ShaderInputType, T>, int> E0>
void HlslShader<T, E0>::BeginDraw() const
{
	DeviceContext->IASetInputLayout(InputLayout);
}

template <class T, enable_if_t<is_base_of_v<ShaderInputType, T>, int> E0>
void HlslShader<T, E0>::EndDraw() const
{
	// 일단 전체 파이프라인에서 HS, DS, GS를 사용하지 않으니 굳이 필요한 코드는 아님.
	// 추후에 다른 RenderPass에서 건드리면 그 때 주석 해제하도록 하자.
	DeviceContext->HSSetShader(nullptr, nullptr, 0);
	DeviceContext->DSSetShader(nullptr, nullptr, 0);
	DeviceContext->GSSetShader(nullptr, nullptr, 0);
}
