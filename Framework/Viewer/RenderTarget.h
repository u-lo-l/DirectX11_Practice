#pragma once

class RenderTarget
{
public:
	RenderTarget(UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat = DXGI_FORMAT_R8G8B8A8_UNORM);
	~RenderTarget();

	operator ID3D11RenderTargetView*() {return RTV;}
	operator const ID3D11RenderTargetView*() const {return RTV;}
	operator ID3D11ShaderResourceView*() {return SRV;}
	operator const ID3D11ShaderResourceView*() const {return SRV;}

	void SaveTexture(const wstring & InTextureFileName) const;
	void SetRenderTarget(const class DepthStencil * InDepthStencil) const;
	void ClearRenderTarget() const;
private:
	UINT Width;
	UINT Height;
	DXGI_FORMAT Format;
	ID3D11Texture2D *Texture;
	ID3D11RenderTargetView *RTV;
	ID3D11ShaderResourceView *SRV;
};
