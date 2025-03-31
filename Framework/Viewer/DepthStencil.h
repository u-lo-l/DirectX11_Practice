#pragma once

class DepthStencil
{
public:
	DepthStencil(UINT InWidth, UINT InHeight, bool bInUseStencil);
	~DepthStencil();
	
	operator ID3D11DepthStencilView*() const {return DSV;}
	operator const ID3D11DepthStencilView*() const {return DSV;}
	operator ID3D11ShaderResourceView*() const {return SRV;}
	operator const ID3D11ShaderResourceView*() const {return SRV;}
	void SaveTexture(const wstring& InTextureFileName);
	void ClearDepthStencil() const;
private:
	bool bUseStencil = false;
	UINT Width;
	UINT Height;
	DXGI_FORMAT Format;
	ID3D11Texture2D *Texture;
	ID3D11DepthStencilView *DSV;
	ID3D11ShaderResourceView *SRV;
};
