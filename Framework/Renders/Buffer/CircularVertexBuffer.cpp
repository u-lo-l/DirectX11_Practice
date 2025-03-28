#include "framework.h"
#include "CircularVertexBuffer.h"

CircularVertexBuffer::CircularVertexBuffer
(
	void* InData,
	UINT InCount,
	UINT InStride,
	UINT InSlot
)
: VertexBuffer(InData, InCount, InStride, InSlot, true)
{
}


void CircularVertexBuffer::UpdateBuffer(void* InData, const UINT StartIndex, const UINT InCount)
{
	if (bCpuWrite == false || InCount == 0)
		return;
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	Data = InData;
	D3D11_MAPPED_SUBRESOURCE Subresource;
	if (FAILED(DeviceContext->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource)))
		return ;


	// if (StartIndex + InCount < this->Count)
	// {
	// 	memcpy(Subresource.pData, ((BYTE*)Data) + StartIndex * Stride, InCount * Stride);
	// }
	// else
	// {
	// 	const UINT FirstPart = Count - StartIndex;  // 끝에서 남은 공간
	// 	memcpy(Subresource.pData, ((BYTE*)Data) + StartIndex * Stride, FirstPart * Stride);
	//
	// 	const UINT SecondPart = InCount - FirstPart;  // 처음부터 복사할 부분
	// 	memcpy((BYTE*)Subresource.pData + FirstPart * Stride, Data, SecondPart * Stride);
	// }
	memcpy(Subresource.pData, Data, Count * Stride);

	DeviceContext->Unmap(Buffer, 0);
}
