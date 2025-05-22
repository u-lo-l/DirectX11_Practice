#include "framework.h"
#include "Buffers.h"

RawBuffer::RawBuffer( void * InInputData, UINT InDataSize, UINT InOutputSize )
	: DataSize(InDataSize), OutputSize(InOutputSize)
{
	Data = InInputData;
	CreateBuffer();
}

void RawBuffer::SetInputData( const void * InData ) const
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE Subresource;
	DeviceContext->Map(Input, 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource);
	memcpy(Subresource.pData, InData, DataSize);
	DeviceContext->Unmap(Input, 0);
}

void RawBuffer::GetOutputData( void * OutData ) const
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	DeviceContext->CopyResource(Result, Output);

	D3D11_MAPPED_SUBRESOURCE Subresource;
	DeviceContext->Map(Result, 0, D3D11_MAP_READ, 0, &Subresource);
	memcpy(OutData, Subresource.pData, OutputSize);
	DeviceContext->Unmap(Result, 0);
}

void RawBuffer::BindInputToGPU() const
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	DeviceContext->CSSetShaderResources(0, 1, &SRV);
}

void RawBuffer::BindOutputToGPU(UINT SlotNum) const
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	// nullptr for Buffer Offset
	DeviceContext->CSSetUnorderedAccessViews(SlotNum, 1, &UAV, nullptr);
}

void RawBuffer::CreateInput()
{
	if (DataSize <= 0)
		return;
	ID3D11Device * Device = D3D::Get()->GetDevice();
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	
	BufferDesc.ByteWidth		= DataSize;
	BufferDesc.BindFlags		= D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.MiscFlags		= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	BufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	BufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA SubresourceData;
	ZeroMemory(&SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	SubresourceData.pSysMem = Data;
		
	CHECK(Device->CreateBuffer(&BufferDesc, Data != nullptr ? &SubresourceData : nullptr, &Buffer) >= 0);

	Input = static_cast<ID3D11Resource *>(Buffer);
}

void RawBuffer::CreateSRV()
{
	if (DataSize <= 0)
		return ;

	ID3D11Buffer * Buffer = nullptr;
	CHECK(Input->QueryInterface(&Buffer) >= 0);
	
	D3D11_BUFFER_DESC BufferDesc;
	Buffer->GetDesc(&BufferDesc);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	SRVDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	SRVDesc.BufferEx.NumElements = BufferDesc.ByteWidth / 4; // 4 : sizeof(float)

	CHECK(D3D::Get()->GetDevice()->CreateShaderResourceView(Buffer, &SRVDesc, &this->SRV) >= 0);
}

void RawBuffer::CreateOutput()
{
	ASSERT(OutputSize > 0, "Invalid OutputSize")
	
	ID3D11Device * Device = D3D::Get()->GetDevice();
	
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	
	BufferDesc.ByteWidth		= OutputSize;
	BufferDesc.BindFlags		= D3D11_BIND_UNORDERED_ACCESS;
	BufferDesc.MiscFlags		= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	BufferDesc.Usage			= D3D11_USAGE_DEFAULT; // GPU에서 CPU로 보내기만 가능.

	ID3D11Buffer * OutBuffer = nullptr;
	CHECK(Device->CreateBuffer(&BufferDesc, nullptr, &OutBuffer) >= 0);

	Output = static_cast<ID3D11Resource *>(OutBuffer);
}

void RawBuffer::CreateUAV()
{
	ASSERT(OutputSize > 0, "Invalid OutputSize")

	ID3D11Buffer * OutBuffer = nullptr;
	CHECK(Output->QueryInterface(&OutBuffer) >= 0);
	
	D3D11_BUFFER_DESC BufferDesc;
	OutBuffer->GetDesc(&BufferDesc);
	
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	UAVDesc.Buffer.NumElements = BufferDesc.ByteWidth / 4; // 4 : sizeof(float)

	CHECK(D3D::Get()->GetDevice()->CreateUnorderedAccessView(OutBuffer, &UAVDesc, &this->UAV) >= 0);
}

void RawBuffer::CreateResult()
{
	ASSERT(OutputSize > 0, "Invalid OutputSize")
	
	ID3D11Device * const Device = D3D::Get()->GetDevice();

	ID3D11Buffer * OutBuffer = nullptr;
	CHECK(Output->QueryInterface(&OutBuffer) >= 0);
	
	D3D11_BUFFER_DESC BufferDesc;
	OutBuffer->GetDesc(&BufferDesc);
	
	BufferDesc.BindFlags		= 0;
	BufferDesc.MiscFlags		= 0;
	BufferDesc.Usage			= D3D11_USAGE_STAGING; // GPU에서 CPU로 보내기만 가능.
	BufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;

	ID3D11Buffer * ResBuffer = nullptr;
	CHECK(Device->CreateBuffer(&BufferDesc, nullptr, &ResBuffer) >= 0);

	Result = static_cast<ID3D11Resource *>(ResBuffer);
}

