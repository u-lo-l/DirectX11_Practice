#include "framework.h"

RWStructuredBuffer::RWStructuredBuffer
(
	ShaderType TargetShaderType,
	int RegisterIndex,
	void* InData,
	UINT InCount,
	UINT InStride
)
: RegisterIndex(RegisterIndex)
, DataSize(InCount * InStride)
, TargetShaderType(TargetShaderType)
{
	Data = InData;
	Count = InCount;
	Stride = InStride;

	CreateUAV();
	CreateSRV();
	CreateResultBuffer();
}

RWStructuredBuffer::~RWStructuredBuffer()
{
	SAFE_RELEASE(ResultBuffer);
	SAFE_RELEASE(SRV);
	SAFE_RELEASE(UAV);
}

void RWStructuredBuffer::BindToGPUAsUAV(UINT SlotNum) const
{
	if (!UAV)
		return;
	D3D::Get()->GetDeviceContext()->CSSetUnorderedAccessViews(SlotNum, 1, &UAV,nullptr);
}

void RWStructuredBuffer::BindToGPUAsSRV(UINT SlotNum) const
{
	if (!SRV)
		return ;
	D3D::Get()->GetDeviceContext()->CSSetShaderResources(SlotNum, 1, &SRV);
}

void RWStructuredBuffer::BindToGPUAsSRV(const UINT SlotNum, const ShaderType InShaderType) const
{
	if (!SRV)
		return ;
	if(InShaderType & ShaderType::VertexShader)
		D3D::Get()->GetDeviceContext()->VSSetShaderResources(SlotNum, 1, &SRV);
	if(InShaderType & ShaderType::PixelShader)
		D3D::Get()->GetDeviceContext()->PSSetShaderResources(SlotNum, 1, &SRV);
	if(InShaderType & ShaderType::HullShader)
		D3D::Get()->GetDeviceContext()->HSSetShaderResources(SlotNum, 1, &SRV);
	if(InShaderType & ShaderType::DomainShader)
		D3D::Get()->GetDeviceContext()->DSSetShaderResources(SlotNum, 1, &SRV);
	if(InShaderType & ShaderType::ComputeShader)
		D3D::Get()->GetDeviceContext()->CSSetShaderResources(SlotNum, 1, &SRV);
}

void RWStructuredBuffer::CreateSRV()
{
	ID3D11Device * Device =  D3D::Get()->GetDevice();

	SAFE_RELEASE(SRV);
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN; // StructuredBuffer일 땐 UNKNOWN
	SRVDesc.Buffer.FirstElement = 0;
	SRVDesc.Buffer.NumElements = Count;
	
	const HRESULT Hr = Device->CreateShaderResourceView(this->Buffer, &SRVDesc, &this->SRV);
	CHECK(SUCCEEDED(Hr));
}

void RWStructuredBuffer::GetResult(void* OutData) const
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	DeviceContext->CopyResource(ResultBuffer, Buffer);

	D3D11_MAPPED_SUBRESOURCE Subresource;
	DeviceContext->Map(ResultBuffer, 0, D3D11_MAP_READ, 0, &Subresource);
	memcpy(OutData, Subresource.pData, Stride * Count);
	DeviceContext->Unmap(ResultBuffer, 0);
}

void RWStructuredBuffer::CreateUAV()
{
	ID3D11Device * Device = D3D::Get()->GetDevice();
	
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.ByteWidth = DataSize;
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.CPUAccessFlags = 0;
	BufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	BufferDesc.StructureByteStride = Stride;

	if (Data == nullptr)
	{
		CHECK(SUCCEEDED(Device->CreateBuffer(&BufferDesc, nullptr, &Buffer)));
	}
	else
	{
		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = Data;
		CHECK(SUCCEEDED(Device->CreateBuffer(&BufferDesc, &InitData, &Buffer)));
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Buffer.NumElements = Count;
	CHECK(Device->CreateUnorderedAccessView(Buffer, &UAVDesc, &UAV) >= 0);

	CreateSRV();
}

void RWStructuredBuffer::CreateResultBuffer()
{
	ID3D11Device * Device =  D3D::Get()->GetDevice();

	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.ByteWidth = DataSize;
	BufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	BufferDesc.StructureByteStride = Stride;
	BufferDesc.Usage = D3D11_USAGE_STAGING;
	BufferDesc.BindFlags = 0;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	BufferDesc.MiscFlags = 0;
	CHECK(Device->CreateBuffer(&BufferDesc, nullptr, &ResultBuffer) >= 0);
}

void RWStructuredBuffer::BindToGPU()
{
	ASSERT(false, "Use BindToGPUAsUAV() or BindToGPUAsSRV()");
}
