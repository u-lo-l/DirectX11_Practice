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
	ASSERT(InDataSize % 16 == 0, "ByteWidth value of D3D11_BUFFER_DESC MUST BE multiples of 16")

	Data = InData;
#ifdef DO_DEBUG
	BufferType = "Constant";
	BufferInfo = move(DataName);
	printf("%s Buffer size %d for %s Created [%s]\n", BufferType.c_str(), InDataSize, DataName.c_str(), BufferInfo.c_str());
#endif
	ID3D11Device * Device = D3D::Get()->GetDevice();
	
	/*  ConstantBuffer 만들기 :
	 * 1. Vertex Shader Constant Data를 기술할 구조체 정의. 이건 BaseBuffer에 있음.
	 * 2. D3D11_BUFFER_DECS 구조체 만들기.
	 * 2-1. BindFlag에는 D3D11_BIND_CONSTANT_BUFFER, ByteWidth 에는 구조체의 크기를 넣어주면 된다.
	 * 3. SubResourceData를 만들기 <- Data가 불변하는 경우는 여기서 해도 된다.
	 * 3-1. D3D11_SUBRESOURCE_DATA구조체의 pSysMem이 ConstantData가 들어있는 구조체의 주소를 가리켜야한다.
	 * 3-2. 불변하는 데이터라면 Usage가 D3D11_USAGE_DEFAULT 또는 D3D11_USAGE_IMMUTABLE기 될 수 있다.
	*/
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	
	BufferDesc.ByteWidth = DataSize;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	/*
	 * you must set the ByteWidth value of D3D11_BUFFER_DESC in multiples of 16
	 * https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createbuffer
	 * https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-buffers-constant-how-to
	 * https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules
	 */
	CHECK(Device->CreateBuffer(&BufferDesc, nullptr, &Buffer) >= 0);
}

void ConstantBuffer::BindToGPU()
{
	/*
	 * Data가 변하는 경우는 변할 때 마다 Map / Unmap 해주어야한다.
	 * [!] Data가 변하기 떄문에 BufferDesc.Usage는 D3D11_USAGE_DYNAMIC 여야 하며
	 * [!] Data가 변하기 떄문에 MapType은 D3D11_MAP_WRITE_DISCARD여야 한다.
	 *
	 * TODO : DynamicConstantBuffer인지 StaticConstantBuffer인지 구분하자.
	 */
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE Subresource;
	// D3D11_MAP_WRITE_DISCARD : 기존 데이터를 버리고 새 데이터를 쓸 때 사용. GPU와 CPU 간 동기화를 방지하고 성능을 최적화하기 위해 기존 데이터를 무시.
	CHECK(DeviceContext->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource) >= 0);
	memcpy(Subresource.pData, this->Data, this->DataSize);
	DeviceContext->Unmap(Buffer, 0);
}

/*=============================================================================*/

ComputeShaderResource::~ComputeShaderResource()
{
	SAFE_RELEASE(Input);
	SAFE_RELEASE(Output);
	SAFE_RELEASE(SRV);
	SAFE_RELEASE(UAV);
	SAFE_RELEASE(Result);
}

void ComputeShaderResource::CreateBuffer()
{
	CreateInput();
	
	CreateSRV();

	CreateOutput();

	CreateUAV();

	CreateResult();
}

RawBuffer::RawBuffer( void * InInputData, string InDataName, UINT InDataSize, UINT InOutputSize )
	: DataSize(InDataSize), OutputSize(InOutputSize)
{
	Data = InInputData;
	CreateBuffer();
}

void RawBuffer::SetInputData( const void * InData )
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE Subresource;
	DeviceContext->Map(Input, 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource);
	memcpy(Subresource.pData, InData, DataSize);
	DeviceContext->Unmap(Input, 0);
}

void RawBuffer::GetOutputData( void * OutData )
{
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();
	DeviceContext->CopyResource(Result, Output);

	D3D11_MAPPED_SUBRESOURCE Subresource;
	DeviceContext->Map(Result, 0, D3D11_MAP_READ, 0, &Subresource);
	memcpy(OutData, Subresource.pData, OutputSize);
	DeviceContext->Unmap(Result, 0);
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

	ID3D11Buffer * Buffer = static_cast<ID3D11Buffer *>(Input);
	D3D11_BUFFER_DESC BufferDesc;
	Buffer->GetDesc(&BufferDesc);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	SRVDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	SRVDesc.BufferEx.NumElements = BufferDesc.ByteWidth / 4; // 4 : sizeof(float)

	CHECK(D3D::Get()->GetDevice()->CreateShaderResourceView(Buffer, &SRVDesc, &this->SRV) >= 0);
	int a = 0;
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
	// BufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;

	ID3D11Buffer * OutBuffer = nullptr;
	CHECK(Device->CreateBuffer(&BufferDesc, nullptr, &OutBuffer) >= 0);

	Output = static_cast<ID3D11Resource *>(OutBuffer);
}

void RawBuffer::CreateUAV()
{
	ASSERT(OutputSize > 0, "Invalid OutputSize")

	// ID3D11Buffer * OutBuffer = dynamic_cast<ID3D11Buffer *>(Output);
	ID3D11Buffer * OutBuffer = static_cast<ID3D11Buffer *>(Output);
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

	// ID3D11Buffer * OutBuffer = dynamic_cast<ID3D11Buffer *>(Output);
	ID3D11Buffer * OutBuffer = static_cast<ID3D11Buffer *>(Output);
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

