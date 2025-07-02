#pragma once

class RenderTarget
{
public:
	RenderTarget(UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat = DXGI_FORMAT_R8G8B8A8_UNORM);
	~RenderTarget();

	// ReSharper disable once CppNonExplicitConversionOperator
	operator ID3D11RenderTargetView*() const {return RTV;}
	// ReSharper disable once CppNonExplicitConversionOperator
	operator const ID3D11RenderTargetView*() const {return RTV;}
	// ReSharper disable once CppNonExplicitConversionOperator
	operator ID3D11ShaderResourceView*() const {return SRV;}
	// ReSharper disable once CppNonExplicitConversionOperator
	operator const ID3D11ShaderResourceView*() const {return SRV;}

	void SaveTexture(const wstring & InTextureFileName) const;
	void SetRenderTarget(const DepthStencil * InDepthStencil) const;
	void ClearRenderTarget() const;
	void ClearRenderTarget(const Color & InColor) const;
	void ResizeRenderTarget(UINT InWidth, UINT InHeight);
private:
	void Init();
	UINT Width;
	UINT Height;
	DXGI_FORMAT Format;
	ID3D11Texture2D * Texture;
	ID3D11RenderTargetView *RTV;
	ID3D11ShaderResourceView *SRV;
};
