#include "framework.h"
#include "Buffers.h"

VertexBuffer::VertexBuffer(
	void * InData,
	UINT InCount,
	UINT InStride,
	UINT InSlot,
	bool InCpuWrite,
	bool InGpuWrite )
: Buffer(nullptr), Data(InData), Count(InCount), Stride(InStride), Slot(InSlot), bCpwWrite(InCpuWrite), bGpuWrite(InGpuWrite)
{
	ID3D11Device * Device = D3D::Get()->GetDevice();
	D3D11_BUFFER_DESC BufferDesc;
	
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.ByteWidth = Stride * Count;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (bCpwWrite == false && bGpuWrite == false)
	{
		BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	}
	else if (bCpwWrite == true && bGpuWrite == false)
	{
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (bCpwWrite == false && bGpuWrite == true)
	{
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		BufferDesc.Usage = D3D11_USAGE_STAGING;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}

	D3D11_SUBRESOURCE_DATA SubresourceData;
	ZeroMemory(&SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	SubresourceData.pSysMem = Data;
	CHECK(Device->CreateBuffer(&BufferDesc, &SubresourceData, &Buffer) >= 0);
}

VertexBuffer::~VertexBuffer()
{
	SAFE_RELEASE(Buffer);
}

void VertexBuffer::BindToGPU() const
{
	ID3D11DeviceContext * DeviceContext = D3D::Get()->GetDeviceContext();
	constexpr UINT Offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &Buffer, &Stride, &Offset);
}

IndexBuffer::IndexBuffer(UINT * InData,	UINT InCount)
: Buffer(nullptr), Data(InData), Count(InCount)
{
	ID3D11Device * Device = D3D::Get()->GetDevice();
	D3D11_BUFFER_DESC BufferDesc;
	D3D11_SUBRESOURCE_DATA SubresourceData;
	
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.ByteWidth = sizeof(UINT) * Count;
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ZeroMemory(&SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	SubresourceData.pSysMem = Data;
	CHECK(Device->CreateBuffer(&BufferDesc, &SubresourceData, &Buffer) >= 0);
}

IndexBuffer::~IndexBuffer()
{
	SAFE_RELEASE(this->Buffer);
}

void IndexBuffer::BindToGPU() const
{
	ID3D11DeviceContext * DeviceContext = D3D::Get()->GetDeviceContext();
	DeviceContext->IASetIndexBuffer(this->Buffer, DXGI_FORMAT_R32_UINT, 0);
}

/*=============================================================================*/
