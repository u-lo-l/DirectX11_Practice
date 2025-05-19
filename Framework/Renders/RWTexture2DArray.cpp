#include "framework.h"
#include "RWTexture2DArray.h"

RWTexture2DArray::RWTexture2DArray(
	UINT InCount,
	UINT InWidth,
	UINT InHeight,
	DXGI_FORMAT InFormat
)
	: ArraySize(InCount), Width(InWidth), Height(InHeight), TextureFormat(InFormat)
{
	CreateOutputTextureAndUAV();
	CreateResultTexture();
}

RWTexture2DArray::~RWTexture2DArray()
{
	SAFE_RELEASE(UAV);
	SAFE_RELEASE(SRV)
}

void RWTexture2DArray::BindToGPUAsUAV(UINT SlotNum) const
{
	if (!!UAV)
		D3D::Get()->GetDeviceContext()->CSSetUnorderedAccessViews(
			SlotNum,
			1,
			&UAV,
			nullptr
		);
}

void RWTexture2DArray::BindToGPUAsSRV(UINT SlotNum, UINT InShaderType) const
{
	if (!SRV)
		return ;
	if(InShaderType & static_cast<UINT>(ShaderType::VertexShader))
		D3D::Get()->GetDeviceContext()->VSSetShaderResources(SlotNum, 1, &SRV);
	if(InShaderType & static_cast<UINT>(ShaderType::PixelShader))
		D3D::Get()->GetDeviceContext()->PSSetShaderResources(SlotNum, 1, &SRV);
	if(InShaderType & static_cast<UINT>(ShaderType::HullShader))
		D3D::Get()->GetDeviceContext()->HSSetShaderResources(SlotNum, 1, &SRV);
	if(InShaderType & static_cast<UINT>(ShaderType::DomainShader))
		D3D::Get()->GetDeviceContext()->DSSetShaderResources(SlotNum, 1, &SRV);
	if(InShaderType & static_cast<UINT>(ShaderType::ComputeShader))
		D3D::Get()->GetDeviceContext()->CSSetShaderResources(SlotNum, 1, &SRV);
}

void RWTexture2DArray::UpdateSRV()
{
	ID3D11Device * Device =  D3D::Get()->GetDevice();

	SAFE_RELEASE(SRV);
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = TextureFormat;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.FirstArraySlice = 0;
	SRVDesc.Texture2DArray.ArraySize = ArraySize;
	SRVDesc.Texture2DArray.MipLevels = 1;
	const HRESULT Hr = Device->CreateShaderResourceView(OutputTextureArray, &SRVDesc, &SRV);
	CHECK(SUCCEEDED(Hr));
}

UINT RWTexture2DArray::GetWidth() const
{ 
	return Width;
}

UINT RWTexture2DArray::GetHeight() const
{ 
	return Height;
}

ID3D11UnorderedAccessView * RWTexture2DArray::GetUAV() const
{
	return UAV;
}

ID3D11ShaderResourceView* RWTexture2DArray::GetSRV() const
{
	return SRV;
}

void RWTexture2DArray::GetSRV(const UINT InSlice, ID3D11ShaderResourceView ** OutSRV) const
{
	ID3D11Device * Device =  D3D::Get()->GetDevice();
	*OutSRV = nullptr;
	D3D11_TEXTURE2D_DESC TextureDesc = {};
	OutputTextureArray->GetDesc(&TextureDesc);
	if (InSlice < 0 || InSlice >= TextureDesc.ArraySize)
	{
		ASSERT(false, "InSlice Out Of Range")
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = TextureDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.FirstArraySlice = InSlice;
	SRVDesc.Texture2DArray.MostDetailedMip = 0;
	SRVDesc.Texture2DArray.ArraySize = 1;
	SRVDesc.Texture2DArray.MipLevels = -1;

	HRESULT Hr = Device->CreateShaderResourceView(OutputTextureArray, &SRVDesc, OutSRV);
	CHECK(SUCCEEDED(Hr));
}

void RWTexture2DArray::SaveOutputAsFile(const wstring& InFileName) const
{
	ID3D11DeviceContext * DeviceContext =  D3D::Get()->GetDeviceContext();
	ID3D11Device * Device=  D3D::Get()->GetDevice();
	DeviceContext->CopyResource(ResultTextureArray, OutputTextureArray);

	DirectX::ScratchImage Image;
	// 내부적으로 InTexture를 Staging으로 변환한다.
	HRESULT Hr = DirectX::CaptureTexture(Device, DeviceContext, ResultTextureArray, Image);
	CHECK(SUCCEEDED(Hr));
	for (int slice = 0 ; slice < ArraySize ; slice++)
	{
		wstring FileName = W_TEXTURE_PATH + InFileName + L"_" + to_wstring(slice) + L".png";
		Hr = DirectX::SaveToWICFile(
			*(Image.GetImage(0,0,slice)),
			DirectX::WIC_FLAGS_NONE,
			DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG),
			FileName.c_str()
		);
		CHECK(SUCCEEDED(Hr));
	}
}

void RWTexture2DArray::CreateOutputTextureAndUAV()
{
	ID3D11Device * Device =  D3D::Get()->GetDevice();
	D3D11_TEXTURE2D_DESC RWTexture2DArrayDesc;
	ZeroMemory(&RWTexture2DArrayDesc, sizeof(D3D11_TEXTURE2D_DESC));
	RWTexture2DArrayDesc.ArraySize = ArraySize;
	RWTexture2DArrayDesc.Width = Width;
	RWTexture2DArrayDesc.Height = Height;
	RWTexture2DArrayDesc.MipLevels = 1;
	RWTexture2DArrayDesc.Format = TextureFormat;
	RWTexture2DArrayDesc.SampleDesc.Count = 1;
	RWTexture2DArrayDesc.SampleDesc.Quality = 0;
	RWTexture2DArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	RWTexture2DArrayDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	RWTexture2DArrayDesc.CPUAccessFlags = 0;
	RWTexture2DArrayDesc.MiscFlags = 0;

	HRESULT Hr = Device->CreateTexture2D(&RWTexture2DArrayDesc, nullptr, &OutputTextureArray);
	CHECK(SUCCEEDED(Hr));

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(UAVDesc));
	UAVDesc.Format = TextureFormat;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	UAVDesc.Texture2DArray.ArraySize = ArraySize;
	UAVDesc.Texture2DArray.MipSlice = 0;
	UAVDesc.Texture2DArray.FirstArraySlice = 0;
	Hr = Device->CreateUnorderedAccessView(OutputTextureArray, &UAVDesc, &UAV);
	CHECK(SUCCEEDED(Hr));
}

void RWTexture2DArray::CreateResultTexture()
{
	ID3D11Device * Device =  D3D::Get()->GetDevice();
	D3D11_TEXTURE2D_DESC RWTexture2DDesc;
	ZeroMemory(&RWTexture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	RWTexture2DDesc.Width = Width;
	RWTexture2DDesc.Height = Height;
	RWTexture2DDesc.MipLevels = 1;
	RWTexture2DDesc.ArraySize = ArraySize;
	RWTexture2DDesc.Format = TextureFormat;
	RWTexture2DDesc.SampleDesc.Count = 1;
	RWTexture2DDesc.SampleDesc.Quality = 0;
	
	RWTexture2DDesc.Usage = D3D11_USAGE_STAGING;
	RWTexture2DDesc.BindFlags = 0;
	RWTexture2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	RWTexture2DDesc.MiscFlags = 0;

	const HRESULT Hr = Device->CreateTexture2D(&RWTexture2DDesc, nullptr, &ResultTextureArray);
	CHECK(SUCCEEDED(Hr));
}
