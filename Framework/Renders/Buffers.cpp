#include "framework.h"
#include "Buffers.h"

BufferBase::~BufferBase()
{
#ifdef DO_DEBUG
	printf("%s : Buffer Safely Released : [%s]\n", BufferType.c_str(), BufferInfo.c_str());
#endif
	SAFE_RELEASE(Buffer);
}

#ifdef DO_DEBUG
VertexBuffer::VertexBuffer(
	void * InData,
	UINT InCount,
	UINT InStride,
	const string & MetaData,
	UINT InSlot,
	bool InCpuWrite,
	bool InGpuWrite
)
: Slot(InSlot), bCpwWrite(InCpuWrite), bGpuWrite(InGpuWrite)
{
	Data = InData;
	Count = InCount;
	Stride = InStride;
	
	BufferType = "Vertex  ";
	BufferInfo = "Size : " + std::to_string(InCount) + " x " + std::to_string(InStride);
	printf("%s Buffer size %d Created : [%s]\n", BufferType.c_str(), Count * Stride, MetaData.c_str());
	
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

#else
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
#endif

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
#ifdef DO_DEBUG
	BufferType = "Index   ";
	BufferInfo = "Size : " + std::to_string(InCount) + " x sizeof(UINT)";
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
 : DataSize(InDataSize), DataName(move(InDataName))
{
	Data = InData;
#ifdef DO_DEBUG
	BufferType = "Constant";
	BufferInfo = move(DataName);
	printf("%s Buffer size %d for %s Created [%s]\n", BufferType.c_str(), InDataSize, DataName.c_str(), BufferInfo.c_str());
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
 * <code>D3D11_MAPPED_SUBRESOURCE</code> : DirectX에서 버퍼에 접근하기 위한 매핑 정보를 저장하는 구조체
 * 
 * Map : GPU 리소스를 CPU가 수정할 수 있도록 매핑.
 *       - <code>BufferDesc.Usage = D3D11_USAGE_DYNAMIC</code>로 설정된 리소스에만 사용 가능.
 *       - <code>D3D11_MAP_WRITE_DISCARD</code>: 기존 데이터를 버리고 새 데이터를 쓰는 방식. 동기화를 방지하여 성능 최적화.
 * 
 * Unmap : GPU가 다시 리소스를 사용할 수 있도록 매핑을 해제.
 *         - 반드시 Unmap을 호출해야 GPU가 데이터를 정상적으로 읽을 수 있음.
 */
void ConstantBuffer::BindToGPU()
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE Subresource;
	// D3D11_MAP_WRITE_DISCARD : 기존 데이터를 버리고 새 데이터를 쓸 때 사용. GPU와 CPU 간 동기화를 방지하고 성능을 최적화하기 위해 기존 데이터를 무시.
	CHECK(DeviceContext->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource) >= 0);
	memcpy(Subresource.pData, this->Data, this->DataSize);
	DeviceContext->Unmap(Buffer, 0);
}

