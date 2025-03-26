#include "framework.h"
#include "HlslComputeShader.h"

HlslComputeShader::HlslComputeShader(const wstring & ShaderFileName, const string& EntryPoint)
{
    if (ShaderFileName.empty() == false)
    {
        FileName = W_SHADER_PATH + ShaderFileName;

        ID3DBlob * ErrorBlob = nullptr;
        ID3DBlob * ShaderBlob = nullptr;

        int Flag = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
        Flag |= D3DCOMPILE_WARNINGS_ARE_ERRORS;

        constexpr int EffectFlag = 0; // Effect FrameWork 쓸 떄만 씀.
	
        HRESULT hr = D3DCompileFromFile(
            FileName.c_str(),
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            EntryPoint.c_str(),
            "cs_5_0",
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
        {
        	string Msg;
        	if (hr == D3D11_ERROR_FILE_NOT_FOUND)
        		Msg = "File not found.";
            ASSERT(false, (String::ToString(ShaderFileName) + " Failed to Compile : Maybe No File or Invalid EntryPoint : " + Msg).c_str())
        }
        SAFE_RELEASE(ErrorBlob);

        ID3D11Device * const Device = D3D::Get()->GetDevice();
        const void * BlobBufferAddr = ShaderBlob->GetBufferPointer();
        const UINT BlobBufferSize = ShaderBlob->GetBufferSize();
        const HRESULT Hr = Device->CreateComputeShader(BlobBufferAddr, BlobBufferSize, nullptr, &ComputeShader);
        SAFE_RELEASE(ShaderBlob);
        ASSERT(Hr >= 0, "Failed to create shader")
    }
}

HlslComputeShader::~HlslComputeShader()
{
	SAFE_RELEASE(ComputeShader);
}

void HlslComputeShader::Dispatch(const RawBuffer * InRawBuffer, UINT X, UINT Y, UINT Z) const
{
	if (!ComputeShader)
		return ;
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	
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