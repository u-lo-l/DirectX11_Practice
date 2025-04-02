#include "framework.h"
#include "DepthStencil.h"

DepthStencil::DepthStencil(UINT InWidth, UINT InHeight, bool bInUseStencil)
	: Width(InWidth), Height(InHeight), bUseStencil(bInUseStencil)
	, Texture(nullptr),	DSV(nullptr), SRV(nullptr)
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	if (Width == 0)
		Width = static_cast<UINT>(D3D::GetDesc().Width);
	if (Height == 0)
		Height = static_cast<UINT>(D3D::GetDesc().Height);
	
	D3D11_TEXTURE2D_DESC TextureDesc {};
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = bUseStencil ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
	TextureDesc.MipLevels = 1;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	CHECK(SUCCEEDED(Device->CreateTexture2D(&TextureDesc, nullptr, &Texture)));

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc {};
	DSVDesc.Format = bUseStencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	CHECK(SUCCEEDED(Device->CreateDepthStencilView(Texture, &DSVDesc, &DSV)));

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc {};
	SRVDesc.Format = bUseStencil ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	CHECK(SUCCEEDED(Device->CreateShaderResourceView(Texture, &SRVDesc, &SRV)));
}

DepthStencil::~DepthStencil()
{
	SAFE_RELEASE(Texture);
	SAFE_RELEASE(SRV);
	SAFE_RELEASE(DSV);
}

void DepthStencil::SaveTexture(const wstring& InTextureFileName)
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	ID3D11DeviceContext * const DeviceContext = D3D::Get()->GetDeviceContext();

	DirectX::ScratchImage Image;
	HRESULT Hr = DirectX::CaptureTexture(Device, DeviceContext, Texture, Image);
	CHECK(SUCCEEDED(Hr));

	Hr = DirectX::SaveToWICFile(
		*(Image.GetImage(0,0,0)),
		DirectX::WIC_FLAGS_NONE,
		DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG),
		(wstring(W_TEXTURE_PATH) + L"_DS_" + InTextureFileName + L".png").c_str()
	);
	CHECK(SUCCEEDED(Hr));
}

void DepthStencil::ClearDepthStencil() const
{
	ID3D11DeviceContext * const Context = D3D::Get()->GetDeviceContext();
	const UINT ClearFlag = bUseStencil ? D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL : D3D11_CLEAR_DEPTH;
	Context->ClearDepthStencilView(DSV, ClearFlag, 1.f, 0);
}
