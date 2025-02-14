#include "framework.h"
#include "VertexBuffer.h"


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
		// 초기 생성 이후 절대 변경 불가
		BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	}
	else if (bCpwWrite == true && bGpuWrite == false)
	{
		// 매 프레임 갱신 되는 데이터
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (bCpwWrite == false && bGpuWrite == true)
	{
		// UpdateSubresource로 데이터를 갱신할 수 있음
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	}
	else // bCpwWrite == true && bCpuRead == true
	{
		// GPU에선 접근 불가하다. CPU에서 데이터를 읽고쓸 수 있다.
		// 그럼 얘는 어디 씀?
		// - CPU에서 텍스쳐를 준비하고 CopyResource로 GPU로 전달 할 수 있다
		// - GPU에서 처리한 데이터를 GPU에서 CPU로 D3D11_USAGE_STAGING으로 복사한 후 Map()으로 읽을 수 있다.
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

	if (bCpwWrite == true)
	{
		D3D11_MAPPED_SUBRESOURCE Subresource;
		DeviceContext->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource);
		memcpy(Subresource.pData, Data, Stride * Count);
		DeviceContext->Unmap(Buffer, 0);
	}
	DeviceContext->IASetVertexBuffers(Slot, 1, &Buffer, &Stride, &Offset);
	
}