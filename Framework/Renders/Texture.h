#pragma once

class Texture
{
public:
public:
	explicit Texture(const wstring& FileName, const DirectX::TexMetadata * InLoadInfo = nullptr, bool bDefaultPath = false);
	~Texture();
public:
	UINT GetWidth() const { return TexMeta.width; }
	UINT GetHeight() const { return TexMeta.height; }
	ID3D11ShaderResourceView * GetSRV() const { return SRV; }
private:
	void LoadMetadata( const DirectX::TexMetadata * InLoadInfo = nullptr);
	void LoadTexture();
public:
	D3D11_TEXTURE2D_DESC ReadPixels(vector<Color>& OutPixels);
	D3D11_TEXTURE2D_DESC ReadPixels(DXGI_FORMAT InFormat, vector<Color> & OutPixels);
	D3D11_TEXTURE2D_DESC ReadPixels(ID3D11Texture2D * InSourceTexture, const DXGI_FORMAT InFormat, vector<Color> & OutPixels);
private:
	static HRESULT LoadTextureFromTexture(
		ID3D11Texture2D * InSourceTexture,
		ID3D11Texture2D ** OutTargetTexture,
		ID3D11ShaderResourceView ** OutTargetSRV
	);
	
private:
	ID3D11ShaderResourceView * SRV;
	DirectX::TexMetadata TexMeta;
	wstring FileName;
};
