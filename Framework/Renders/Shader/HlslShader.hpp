#pragma once
#include "framework.h"

template <class T>
std::string HlslShader<T>::GetShaderTarget( ShaderType Type )
{
	switch (Type)
	{
	case ShaderType::VertexShader:	return "vs_5_0";
	case ShaderType::PixelShader:	return "ps_5_0";
	case ShaderType::ComputeShader:	return "cs_5_0";
	default:
		ASSERT(false, "Unknown Shader Type")
		return "";
	}
}

template <class T>
std::string HlslShader<T>::GetEntryPoint( ShaderType Type )
{
	switch (Type)
	{
	case ShaderType::VertexShader:	return "VSMain";
	case ShaderType::PixelShader:	return "PSMain";
	case ShaderType::ComputeShader:	return "CSMain";
	default :
		ASSERT(false, "Unknown Shader Type")
		return "";
	}
}

template <class T>
HlslShader<T>::HlslShader(const wstring & ShaderFileName)
	: DeviceContext(D3D::Get()->GetDeviceContext())
	, InputLayout(nullptr)
	, VertexShader(nullptr)
	, PixelShader(nullptr)
	, ComputeShader(nullptr)
	, RasterizerState(nullptr)
{
	if (ShaderFileName.empty() == false)
	{
		FileName = W_SHADER_PATH + ShaderFileName;
		CompileShader(ShaderType::VertexShader, FileName);
		CompileShader(ShaderType::PixelShader, FileName);
		// CompileShader(ShaderType::ComputeShader, FileName);
	}
}

template <class T>
HlslShader<T>::~HlslShader()
{
	SAFE_RELEASE(InputLayout);
	SAFE_RELEASE(VertexShader);
	SAFE_RELEASE(PixelShader);
	SAFE_RELEASE(ComputeShader);
	SAFE_RELEASE(RasterizerState);
}

template <class T>
void HlslShader<T>::Draw( UINT VertexCount, UINT StartVertexLocation ) const
{
	BeginDraw();
	DeviceContext->Draw(VertexCount, StartVertexLocation);
	EndDraw();
}

template <class T>
void HlslShader<T>::DrawIndexed( UINT IndexCount, UINT StartIndexLocation, UINT BaseVertexLocation ) const
{
	BeginDraw();
	DeviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
	EndDraw();
}

template <class T>
void HlslShader<T>::DrawInstanced( UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation) const
{
	BeginDraw();
	DeviceContext->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
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

template <class T>
void HlslShader<T>::Dispatch( UINT X, UINT Y, UINT Z ) const
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


template <class T>
int HlslShader<T>::AddConstantBuffer( ID3D11Buffer * CBuffer )
{
	const int index = ConstantBuffers.size();
	ConstantBuffers.push_back(CBuffer);
	DeviceContext->VSSetConstantBuffers(index, 1, &CBuffer);
	return index;	
}

template <class T>
HRESULT HlslShader<T>::SetRasterizerState( const D3D11_RASTERIZER_DESC * RSDesc )
{
	return D3D::Get()->GetDevice()->CreateRasterizerState( RSDesc, &this->RasterizerState );
}

template <class T>
void HlslShader<T>::CompileShader( ShaderType Type, const wstring & ShaderFileName )
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
	{
		const char * const ErrMsg = static_cast<char *>(ErrorBlob->GetBufferPointer());
		ASSERT(false, (String::ToString(ShaderFileName) + " Failed to Compile :\n" + "<" + ErrMsg + ">").c_str())
	}
	if (FAILED(hr) && ErrorBlob == nullptr)
		ASSERT(false, (String::ToString(ShaderFileName) + " Failed to Compile : Maybe No File or Invalid EntryPoint").c_str())
	SAFE_RELEASE(ErrorBlob);
	
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	HRESULT Hr = 0;
	const void * BufferAddr = ShaderBlob->GetBufferPointer();
	const UINT BufferSize = ShaderBlob->GetBufferSize();
	if (Type == ShaderType::VertexShader)
	{
		Hr = Device->CreateVertexShader(BufferAddr, BufferSize, nullptr, &VertexShader);
		InitializeInputLayout(ShaderBlob);
	}
	else if (Type == ShaderType::PixelShader)
	{
		Hr = Device->CreatePixelShader(BufferAddr, BufferSize, nullptr, &PixelShader);
	}
	else if (Type == ShaderType::ComputeShader)
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
	ASSERT((Hr >= 0), "Failed to create input layout")
}

template <class T>
void HlslShader<T>::BeginDraw() const
{
	DeviceContext->IASetInputLayout(InputLayout);

	DeviceContext->CSSetShader(ComputeShader, nullptr, 0);
	DeviceContext->VSSetShader(VertexShader, nullptr, 0);
	DeviceContext->RSSetState(RasterizerState);
	DeviceContext->PSSetShader(PixelShader, nullptr, 0);
}

template <class T>
void HlslShader<T>::EndDraw() const
{
	// 일단 전체 파이프라인에서 HS, DS, GS를 사용하지 않으니 굳이 필요한 코드는 아님.
	// 추후에 다른 RenderPass에서 건드리면 그 때 주석 해제하도록 하자.
	// DeviceContext->HSSetShader(nullptr, nullptr, 0);
	// DeviceContext->DSSetShader(nullptr, nullptr, 0);
	// DeviceContext->GSSetShader(nullptr, nullptr, 0);
}
