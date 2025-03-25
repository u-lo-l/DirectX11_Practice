#include "framework.h"
#include "VertexBuffer.h"

/*	CpuWrite과 GpuWrite
 *	bCPUAccess == X && bGPUAccess ==  R : Immutable = 초기 생성 이후 변경 불가
 *	bCpUAccess == W && bGPUAccess ==  R : Dynamic   = 매 프레임 변경되는 데이터
 *	bCpUAccess == X && bGPUAccess == RW : Default   = GPU에서만 읽고 씀. Map할 수 없고 UpdateSubresource써야함.
 *	bCpUAccess == R && bGpuAccess ==  X : Staging   = GPU에서 처리한 결과를 CPU로 복사하기 위함.
 *	-> CpuAccessFlag는 BufferDesc.CPUAccessFlags로 지정해주고
 *	-> GpuAccessFlag는 BufferDesc.Usage로 지정해준다.
 *	두 조합이 맞지 않으면 런타임에서 터진다.
 */
VertexBuffer::VertexBuffer
(
	void * InData,
	UINT InCount,
	UINT InStride,
	UINT InSlot,
	bool InCpuWrite,
	bool InGpuWrite
)
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

InstanceBuffer::InstanceBuffer( void * InData, UINT InCount, UINT InStride )
	: VertexBuffer(InData, InCount, InStride, 9, true)
{
	
}
