#include "framework.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat)
	: Width(InWidth), Height(InHeight), Format(InFormat)
	, Texture(nullptr), RTV(nullptr), SRV(nullptr)
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	if (Width == 0)
		Width = static_cast<UINT>(D3D::GetDesc().Width);
	if (Height == 0)
		Height = static_cast<UINT>(D3D::GetDesc().Height);
	
	D3D11_TEXTURE2D_DESC TextureDesc {};
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.MipLevels = 3;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = Format;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	CHECK(SUCCEEDED(Device->CreateTexture2D(&TextureDesc, nullptr, &Texture)));

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc {};
	RTVDesc.Format = Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	CHECK(SUCCEEDED(Device->CreateRenderTargetView(Texture, &RTVDesc, &RTV)));

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc {};
	SRVDesc.Format = Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	CHECK(SUCCEEDED(Device->CreateShaderResourceView(Texture, &SRVDesc, &SRV)));
}

RenderTarget::~RenderTarget()
{
	SAFE_RELEASE(Texture);
	SAFE_RELEASE(SRV);
	SAFE_RELEASE(RTV);
}

void RenderTarget::SaveTexture(const wstring& InTextureFileName) const
{
	const wstring FileName = L"_RT_" + InTextureFileName;
	Texture::SaveTextureAsFile(Texture, FileName);
}

void RenderTarget::SetRenderTarget(const DepthStencil* InDepthStencil) const
{
	D3D::Get()->GetDeviceContext()->OMSetRenderTargets(1, &RTV, *InDepthStencil);
}

void RenderTarget::ClearRenderTarget() const
{
	D3D::Get()->GetDeviceContext()->ClearRenderTargetView(RTV,D3D::GetDesc().Background);
}

void RenderTarget::ClearRenderTarget(const Color& InColor) const
{
	D3D::Get()->GetDeviceContext()->ClearRenderTargetView(RTV, InColor);
}
