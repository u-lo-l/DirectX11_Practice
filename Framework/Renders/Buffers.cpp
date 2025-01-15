#include "framework.h"
#include "Buffers.h"

BufferBase::~BufferBase()
{
#ifdef _DEBUG
	printf("%s : Buffer Safely Released\n", BufferType.c_str());
#endif
	SAFE_RELEASE(Buffer);
}

VertexBuffer::VertexBuffer(
	void * InData,
	UINT InCount,
	UINT InStride,
	UINT InSlot,
	bool InCpuWrite,
	bool InGpuWrite )
: Slot(InSlot), bCpwWrite(InCpuWrite), bGpuWrite(InGpuWrite)
{
	Data = InData;
	Count = InCount;
	Stride = InStride;
#ifdef _DEBUG
	BufferType = "Vertex";
	printf("%s Buffer size %d Created\n", BufferType.c_str(), Count * Stride);
#endif
	
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

void VertexBuffer::BindToGPU()
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	constexpr UINT Offset = 0;
	DeviceContext->IASetVertexBuffers(Slot, 1, &Buffer, &Stride, &Offset);
}

IndexBuffer::IndexBuffer(UINT * InData,	UINT InCount)
{
	Data = InData;
	Count = InCount;
	Stride = sizeof(UINT);
#ifdef _DEBUG
	BufferType = "Index";
	printf("%s Buffer size %d Created\n", BufferType.c_str(), Count * Stride);
#endif
	ID3D11Device * Device = D3D::Get()->GetDevice();
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	
	D3D11_SUBRESOURCE_DATA SubresourceData;
	BufferDesc.ByteWidth = Stride * Count;
	
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ZeroMemory(&SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	SubresourceData.pSysMem = Data;
	CHECK(Device->CreateBuffer(&BufferDesc, &SubresourceData, &Buffer) >= 0);
}

void IndexBuffer::BindToGPU()
{
	ID3D11DeviceContext * DeviceContext = D3D::Get()->GetDeviceContext();
	DeviceContext->IASetIndexBuffer(this->Buffer, DXGI_FORMAT_R32_UINT, 0);
}

/*=============================================================================*/

ConstantBuffer::ConstantBuffer(void * InData, string InDataName, UINT InDataSize)
 : DataSize(InDataSize), DataName(std::move(InDataName))
{
	Data = InData;
#ifdef _DEBUG
	BufferType = "Constant";
	printf("%s Buffer size %d for %s Created\n", BufferType.c_str(), InDataSize, DataName.c_str());
#endif
	ID3D11Device * Device = D3D::Get()->GetDevice();
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	
	BufferDesc.ByteWidth = DataSize;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	CHECK(Device->CreateBuffer(&BufferDesc, nullptr, &this->Buffer) >= 0);
}

/**
 * D3D11_MAPPED_SUBRESOURCE : DX에서 버퍼에 접근하기 위한 매핑정보를 저장하는 구조체
 * Map : GPU에 있는 리소스를 CPU가 수정할 수 있도록 매핑함. 따라서 BufferDesc.Usage = D3D11_USAGE_DYNAMIC로 설정함.
 * Unmap : Unmap 해줘야 GPU에서 다시 리소스를 사용할 수 있다.
 */
void ConstantBuffer::BindToGPU()
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE Subresource;
	CHECK(DeviceContext->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource) >= 0);
	memcpy(Subresource.pData, Data, DataSize);
	DeviceContext->Unmap(Buffer, 0);
}

