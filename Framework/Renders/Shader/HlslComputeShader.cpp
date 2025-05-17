#include "framework.h"
#include "HlslComputeShader.h"

HlslComputeShader::HlslComputeShader
(
	const wstring & ShaderFileName,
	const D3D_SHADER_MACRO * InMacros,
	const string& EntryPoint
)
{
    if (ShaderFileName.empty() == false)
    {
        FileName = W_SHADER_PATH + ShaderFileName;

        ID3DBlob * ErrorBlob = nullptr;
        ID3DBlob * ShaderBlob = nullptr;

        int Flag = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
        Flag |= D3DCOMPILE_WARNINGS_ARE_ERRORS;

        HRESULT Hr = D3DCompileFromFile(
            FileName.c_str(),
            InMacros,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            EntryPoint.c_str(),
            "cs_5_0",
            Flag,
            0,
            &ShaderBlob,
            &ErrorBlob
        );
        if (FAILED(Hr) && ErrorBlob != nullptr)
        {
            const char * const ErrMsg = static_cast<char *>(ErrorBlob->GetBufferPointer());
            ASSERT(false, (String::ToString(ShaderFileName) + " Failed to Compile :\n" + "<" + ErrMsg + ">").c_str())
        }
        if (FAILED(Hr) && ErrorBlob == nullptr)
        {
        	string Msg;
        	if (Hr == D3D11_ERROR_FILE_NOT_FOUND)
        		Msg = "File not found.";
            ASSERT(false, (String::ToString(ShaderFileName) + " Failed to Compile : Maybe No File or Invalid EntryPoint : " + Msg).c_str())
        }
        SAFE_RELEASE(ErrorBlob);

        ID3D11Device * const Device = D3D::Get()->GetDevice();
        const void * BlobBufferAddr = ShaderBlob->GetBufferPointer();
        const UINT BlobBufferSize = ShaderBlob->GetBufferSize();
        Hr = Device->CreateComputeShader(BlobBufferAddr, BlobBufferSize, nullptr, &ComputeShader);
        SAFE_RELEASE(ShaderBlob);
        ASSERT(Hr >= 0, "Failed to create shader")
    }
}

HlslComputeShader::~HlslComputeShader()
{
	SAFE_RELEASE(ComputeShader);
}

void HlslComputeShader::SetDispatchSize(UINT X, UINT Y, UINT Z)
{
	DispatchSize[0] = X;
	DispatchSize[1] = Y;
	DispatchSize[2] = Z;
}

void HlslComputeShader::Dispatch() const
{
	this->Dispatch(DispatchSize[0], DispatchSize[1], DispatchSize[2]);
}

HRESULT HlslComputeShader::CreateSamplerState_Anisotropic()
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

	return this->CreateSamplerState(&SamplerDesc, SamplerStateType::Anisotropic);
}

HRESULT HlslComputeShader::CreateSamplerState_Anisotropic_Clamp()
{
	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	SamplerDesc.MaxAnisotropy = 4;

	return this->CreateSamplerState(&SamplerDesc, SamplerStateType::Anisotropic);
}

HRESULT HlslComputeShader::CreateSamplerState_Anisotropic_Wrap()
{
	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	SamplerDesc.MaxAnisotropy = 4;

	return this->CreateSamplerState(&SamplerDesc, SamplerStateType::Anisotropic);
}

HRESULT HlslComputeShader::CreateSamplerState_ShadowSampler()
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

	return this->CreateSamplerState(&SamplerDesc, SamplerStateType::Shadow);
}


HRESULT HlslComputeShader::CreateSamplerState_Linear_Wrap()
{
	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;  // 주소 모드 설정 (기본값: 반복)
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;  // 비교 함수 설정 (기본값: 사용 안함)
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	return this->CreateSamplerState(&SamplerDesc, SamplerStateType::Linear);
}

HRESULT HlslComputeShader::CreateSamplerState_Linear_Clamp()
{
	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;  // 주소 모드 설정 (기본값: 반복)
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;  // 비교 함수 설정 (기본값: 사용 안함)
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	return this->CreateSamplerState(&SamplerDesc, SamplerStateType::Linear);
}

HRESULT HlslComputeShader::CreateSamplerState_Linear_Border()
{
	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;  // 주소 모드 설정 (기본값: 반복)
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;  // 비교 함수 설정 (기본값: 사용 안함)
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	return this->CreateSamplerState(&SamplerDesc, SamplerStateType::Linear);
}

HRESULT HlslComputeShader::CreateSamplerState( const D3D11_SAMPLER_DESC * SampDesc, const SamplerStateType SamplerType)
{
	SAFE_RELEASE(SamplerStates[(UINT)SamplerType]);
	return D3D::Get()->GetDevice()->CreateSamplerState(SampDesc, &SamplerStates[static_cast<UINT>(SamplerType)]);
}

void HlslComputeShader::Dispatch(const RawBuffer * InRawBuffer, UINT X, UINT Y, UINT Z) const
{
	if (!ComputeShader)
		return ;
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	for (UINT i = 0 ; i < (UINT)SamplerStateType::Max ; i++)
	{
		ID3D11SamplerState * TargetSampler = (SamplerStates[i]);
		DeviceContext->CSSetSamplers(i, 1, &TargetSampler);
	}
	
	DeviceContext->CSSetShader(ComputeShader, nullptr, 0);
	InRawBuffer->BindInputToGPU();
	InRawBuffer->BindOutputToGPU();
	
	DeviceContext->Dispatch(X, Y, Z);

	// Best-Practice임. 하지만 근거는 이해하지 못함.
	// https://github.com/walbourn/directx-sdk-samples/blob/main/BasicCompute11/BasicCompute11.cpp
	ID3D11ShaderResourceView * NullSRV = nullptr;
	DeviceContext->CSSetShaderResources(0, 1, &NullSRV);
	ID3D11UnorderedAccessView * NullUAV = nullptr;
	DeviceContext->CSSetUnorderedAccessViews(0,1, &NullUAV, nullptr);
	DeviceContext->CSSetShader(nullptr, nullptr, 0);
}

void HlslComputeShader::Dispatch(UINT X, UINT Y, UINT Z) const
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	DeviceContext->CSSetShader(ComputeShader, nullptr, 0);
	for (UINT i = 0 ; i < (UINT)SamplerStateType::Max ; i++)
	{
		ID3D11SamplerState * TargetSampler = (SamplerStates[i]);
		DeviceContext->CSSetSamplers(i, 1, &TargetSampler);
	}
	
	DeviceContext->Dispatch(X, Y, Z);

	ID3D11ShaderResourceView * NullSRV = nullptr;
	DeviceContext->CSSetShaderResources(0, 1, &NullSRV);
	ID3D11UnorderedAccessView * NullUAV = nullptr;
	DeviceContext->CSSetUnorderedAccessViews(0,1, &NullUAV, nullptr);
	DeviceContext->CSSetShader(nullptr, nullptr, 0);
}
