#include "framework.h"
#include "IndexBuffer.h"

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
