#include "framework.h"
#include "RWTexture2D.h"


RWTexture2D::RWTexture2D(const UINT Width, const UINT Height, const DXGI_FORMAT InTextureFormat)
	: Width(Width), Height(Height), TextureFormat(InTextureFormat)
{
	CreateOutputTextureAndUAV();
	CreateResultTexture();
}

RWTexture2D::~RWTexture2D()
{
	SAFE_RELEASE(UAV);
	SAFE_RELEASE(SRV);
}

void RWTexture2D::BindToGPUAsUAV(const UINT SlotNum) const
{
	if (!!UAV)
		D3D::Get()->GetDeviceContext()->CSSetUnorderedAccessViews(
			SlotNum,
			1,
			&UAV,
			nullptr
		);
}

void RWTexture2D::BindToGPUAsSRV(const UINT SlotNum) const
{
	if (!SRV)
		return ;
	D3D::Get()->GetDeviceContext()->CSSetShaderResources(SlotNum, 1, &SRV);
}

void RWTexture2D::BindToGPUAsSRV(const UINT SlotNum, const UINT InShaderType) const
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

void RWTexture2D::UpdateSRV()
{
	ID3D11Device * Device =  D3D::Get()->GetDevice();

	SAFE_RELEASE(SRV);
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = TextureFormat;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	const HRESULT Hr = Device->CreateShaderResourceView(OutputTexture, &SRVDesc, &SRV);
	CHECK(SUCCEEDED(Hr));
}

UINT RWTexture2D::GetWidth() const
{ 
 	return Width;
}

UINT RWTexture2D::GetHeight() const
{ 
 	return Height;
}

ID3D11UnorderedAccessView * RWTexture2D::GetUAV() const
{
	return UAV;
}

ID3D11ShaderResourceView* RWTexture2D::GetSRV() const
{
	return SRV;
}

void RWTexture2D::SaveOutputAsFile(const wstring& FileName) const
{
	D3D11_TEXTURE2D_DESC TextureDesc;
	OutputTexture->GetDesc(&TextureDesc);
	if (
		TextureDesc.Format != DXGI_FORMAT_R8G8B8A8_UNORM ||
		TextureDesc.Format != DXGI_FORMAT_R32G32B32A32_FLOAT ||
		TextureDesc.Format != DXGI_FORMAT_R32_FLOAT
	)
		ASSERT(false, "Texture format does not valid to save as file");
		
	ID3D11DeviceContext * DeviceContext =  D3D::Get()->GetDeviceContext();
	DeviceContext->CopyResource(ResultTexture, OutputTexture);
	Texture::SaveTextureAsFile(ResultTexture, FileName);
}

void RWTexture2D::CreateOutputTextureAndUAV()
{
	ID3D11Device * Device =  D3D::Get()->GetDevice();
	D3D11_TEXTURE2D_DESC RWTexture2DDesc;
	ZeroMemory(&RWTexture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	RWTexture2DDesc.Width = Width;
	RWTexture2DDesc.Height = Height;
	RWTexture2DDesc.MipLevels = 1;
	RWTexture2DDesc.ArraySize = 1;
	RWTexture2DDesc.Format = TextureFormat;
	RWTexture2DDesc.SampleDesc.Count = 1;
	RWTexture2DDesc.SampleDesc.Quality = 0;
	
	RWTexture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	// RW
	RWTexture2DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	// Write Only
	// RWTexture2DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	RWTexture2DDesc.CPUAccessFlags = 0;
	RWTexture2DDesc.MiscFlags = 0;

	HRESULT Hr = Device->CreateTexture2D(&RWTexture2DDesc, nullptr, &OutputTexture);
	CHECK(SUCCEEDED(Hr));

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(UAVDesc));
	UAVDesc.Format = TextureFormat;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVDesc.Texture2D.MipSlice = 0;
	Hr = Device->CreateUnorderedAccessView(OutputTexture, &UAVDesc,	&UAV);
	CHECK(SUCCEEDED(Hr));

	// D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	// ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	// SRVDesc.Format = TextureFormat;
	// SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	// SRVDesc.Texture2D.MipLevels = 1;
	// SRVDesc.Texture2D.MostDetailedMip = 0;
	// Hr = Device->CreateShaderResourceView(OutputTexture, &SRVDesc,	&SRV);
	// CHECK(SUCCEEDED(Hr));
}

void RWTexture2D::CreateResultTexture()
{
	ID3D11Device * Device =  D3D::Get()->GetDevice();
	D3D11_TEXTURE2D_DESC RWTexture2DDesc;
	ZeroMemory(&RWTexture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	RWTexture2DDesc.Width = Width;
	RWTexture2DDesc.Height = Height;
	RWTexture2DDesc.MipLevels = 1;
	RWTexture2DDesc.ArraySize = 1;
	RWTexture2DDesc.Format = TextureFormat;
	RWTexture2DDesc.SampleDesc.Count = 1;
	RWTexture2DDesc.SampleDesc.Quality = 0;
	
	RWTexture2DDesc.Usage = D3D11_USAGE_STAGING;
	RWTexture2DDesc.BindFlags = 0;
	RWTexture2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	RWTexture2DDesc.MiscFlags = 0;

	const HRESULT Hr = Device->CreateTexture2D(&RWTexture2DDesc, nullptr, &ResultTexture);
	CHECK(SUCCEEDED(Hr));
}
