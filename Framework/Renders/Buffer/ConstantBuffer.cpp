#include "framework.h"
#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer
(
	const ShaderType TargetShaderType,
	const int RegisterIndex,
	void* InData,
	const string& InDataName,
	const UINT InDataSize,
	const bool bStatic
)
	: RegisterIndex(RegisterIndex), DataSize(InDataSize), DataName(move(InDataName)), bIsStatic(bStatic), TargetShaderType(TargetShaderType)
{
	ASSERT(InDataSize % 16 == 0, "ByteWidth value of D3D11_BUFFER_DESC MUST BE multiples of 16")

	Data = InData;
	ID3D11Device * Device = D3D::Get()->GetDevice();
	
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	
	BufferDesc.ByteWidth = DataSize;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (bIsStatic == false)
	{
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	}
	
	if (Data == nullptr)
		CHECK(Device->CreateBuffer(&BufferDesc, nullptr, &Buffer) >= 0);
	else
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = this->Data;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		CHECK(Device->CreateBuffer(&BufferDesc, &InitData, &Buffer) >= 0);
	}
}

ConstantBuffer::ConstantBuffer
(
	ShaderType TargetShaderType,
	int RegisterIndex,
	void* InData,
	UINT InDataSize,
	bool bStatic
)
	: ConstantBuffer(TargetShaderType, RegisterIndex, InData, "", InDataSize, bStatic)
{
}

ConstantBuffer::~ConstantBuffer()
= default;

void ConstantBuffer::UpdateData( void * InData, UINT InDataSize )
{
	if (bIsStatic == true)
		return ;
	if (DataSize != InDataSize)
		return;
	Data = InData;

	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE Subresource;
	// D3D11_MAP_WRITE_DISCARD : 기존 데이터를 버리고 새 데이터를 쓸 때 사용. GPU와 CPU 간 동기화를 방지하고 성능을 최적화하기 위해 기존 데이터를 무시.
	CHECK(DeviceContext->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource) >= 0);
	memcpy(Subresource.pData, this->Data, this->DataSize);
	DeviceContext->Unmap(Buffer, 0);
}

void ConstantBuffer::BindToGPU()
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	if (TargetShaderType & ShaderType::VertexShader)
		DeviceContext->VSSetConstantBuffers(RegisterIndex, 1, &Buffer);
	if (TargetShaderType & ShaderType::PixelShader)
		DeviceContext->PSSetConstantBuffers(RegisterIndex, 1, &Buffer);
	if (TargetShaderType & ShaderType::HullShader)
		DeviceContext->HSSetConstantBuffers(RegisterIndex, 1, &Buffer);
	if (TargetShaderType & ShaderType::DomainShader)
		DeviceContext->DSSetConstantBuffers(RegisterIndex, 1, &Buffer);
	if (TargetShaderType & ShaderType::GeometryShader)
		DeviceContext->GSSetConstantBuffers(RegisterIndex, 1, &Buffer);
	if (TargetShaderType & ShaderType::ComputeShader)
		DeviceContext->CSSetConstantBuffers(RegisterIndex, 1, &Buffer);
}

void ConstantBuffer::BindToGPU(int InRegisterIndex) const
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	if (TargetShaderType & ShaderType::VertexShader)
		DeviceContext->VSSetConstantBuffers(InRegisterIndex, 1, &Buffer);
	if (TargetShaderType & ShaderType::PixelShader)
		DeviceContext->PSSetConstantBuffers(InRegisterIndex, 1, &Buffer);
	if (TargetShaderType & ShaderType::HullShader)
		DeviceContext->HSSetConstantBuffers(InRegisterIndex, 1, &Buffer);
	if (TargetShaderType & ShaderType::DomainShader)
		DeviceContext->DSSetConstantBuffers(InRegisterIndex, 1, &Buffer);
	if (TargetShaderType & ShaderType::GeometryShader)
		DeviceContext->GSSetConstantBuffers(InRegisterIndex, 1, &Buffer);
	if (TargetShaderType & ShaderType::ComputeShader)
		DeviceContext->CSSetConstantBuffers(InRegisterIndex, 1, &Buffer);
}

void ConstantBuffer::BindToGPU(ShaderType TargetShader, int InRegisterIndex) const
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	if (TargetShader & ShaderType::VertexShader)
		DeviceContext->VSSetConstantBuffers(InRegisterIndex, 1, &Buffer);
	if (TargetShader & ShaderType::PixelShader)
		DeviceContext->PSSetConstantBuffers(InRegisterIndex, 1, &Buffer);
	if (TargetShader & ShaderType::HullShader)
		DeviceContext->HSSetConstantBuffers(InRegisterIndex, 1, &Buffer);
	if (TargetShader & ShaderType::DomainShader)
		DeviceContext->DSSetConstantBuffers(InRegisterIndex, 1, &Buffer);
	if (TargetShader & ShaderType::GeometryShader)
		DeviceContext->GSSetConstantBuffers(InRegisterIndex, 1, &Buffer);
	if (TargetShader & ShaderType::ComputeShader)
		DeviceContext->CSSetConstantBuffers(InRegisterIndex, 1, &Buffer);
}
