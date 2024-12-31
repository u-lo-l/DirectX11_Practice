#pragma once

class Texture
{
public:
	explicit Texture(const wstring& FileName, D3DX11_IMAGE_INFO * InLoadInfo = nullptr);
	~Texture();
public:
	__forceinline UINT GetWidth() const { return TexMeta.width; }
	__forceinline UINT GetHeight() const { return TexMeta.height; }
private:
	void LoadMetadata(D3DX11_IMAGE_INFO * InLoadInfo = nullptr);
	void LoadTexture();
public:
	D3D11_TEXTURE2D_DESC ReadPixels(vector<Color>& OutPixels);
	D3D11_TEXTURE2D_DESC ReadPixels(DXGI_FORMAT InFormat, vector<Color> & OutPixels);
	D3D11_TEXTURE2D_DESC ReadPixels(ID3D11Texture2D * InSourceTexture, const DXGI_FORMAT InFormat, vector<Color> & OutPixels);

private:
	ID3D11ShaderResourceView * SRV;
	DirectX::TexMetadata TexMeta;
	wstring FileName;
};
