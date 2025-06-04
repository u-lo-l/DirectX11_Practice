#include "framework.h"
#include "StructuredBuffer.h"

StructuredBuffer::StructuredBuffer
(
	UINT TargetShaderType,
	int RegisterIndex,
	void * InData,
	UINT InCount,
	UINT InStride,
	bool bStatic
)
 : RegisterIndex(RegisterIndex)
 , DataSize(InCount * InStride)
 , bIsStatic(bStatic)
 , TargetShaderType(TargetShaderType)
{
	Data = InData;
	Count = InCount;
	Stride = InStride;
	ID3D11Device * Device = D3D::Get()->GetDevice();

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.ByteWidth = DataSize;
	BufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	BufferDesc.StructureByteStride = Stride;
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
		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = Data;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		CHECK(Device->CreateBuffer(&BufferDesc, &InitData, &Buffer) >= 0);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // StructuredBuffer일 땐 UNKNOWN
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = Count;

	Device->CreateShaderResourceView(this->Buffer, &srvDesc, &this->SRV);
}

StructuredBuffer::~StructuredBuffer()
{
	SAFE_RELEASE(this->SRV);
	SAFE_RELEASE(this->Buffer);
}

void StructuredBuffer::UpdateData(void* InData, UINT InDataSize)
{
	if (bIsStatic == true)
		return ;
	if (DataSize != InDataSize)
		return;
	Data = InData;

	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE Subresource;
	CHECK(DeviceContext->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource) >= 0);
	memcpy(Subresource.pData, this->Data, this->DataSize);
	DeviceContext->Unmap(Buffer, 0);
}

void StructuredBuffer::BindToGPU()
{
	ID3D11DeviceContext * DeviceContext = D3D::Get()->GetDeviceContext();

	if (TargetShaderType & static_cast<UINT>(ShaderType::VertexShader))
		DeviceContext->VSSetShaderResources(RegisterIndex, 1, &SRV);
	if (TargetShaderType & static_cast<UINT>(ShaderType::PixelShader))
		DeviceContext->PSSetShaderResources(RegisterIndex, 1, &SRV);
	if (TargetShaderType & static_cast<UINT>(ShaderType::HullShader))
		DeviceContext->HSSetShaderResources(RegisterIndex, 1, &SRV);
	if (TargetShaderType & static_cast<UINT>(ShaderType::DomainShader))
		DeviceContext->DSSetShaderResources(RegisterIndex, 1, &SRV);
	if (TargetShaderType & static_cast<UINT>(ShaderType::GeometryShader))
		DeviceContext->GSSetShaderResources(RegisterIndex, 1, &SRV);
	if (TargetShaderType & static_cast<UINT>(ShaderType::ComputeShader))
		DeviceContext->CSSetShaderResources(RegisterIndex, 1, &SRV);
}
