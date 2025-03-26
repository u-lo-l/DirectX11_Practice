#pragma once

class Texture
{
public:
public:
	explicit Texture(const wstring& FileName, bool bDefaultPath = false);
	~Texture();
public:
	void BindToGPU() const;;
	UINT GetWidth() const { return TexMeta.width; }
	UINT GetHeight() const { return TexMeta.height; }
	ID3D11ShaderResourceView * GetSRV() const { return SRV; }
	operator ID3D11ShaderResourceView * const *() const { return &SRV; }
	// D3D11_TEXTURE2D_DESC ReadPixels(vector<Color>& OutPixels);
	void ExtractTextureColors(vector<Color>& OutPixels) const;
private:
	HRESULT LoadTextureAndCreateSRV(const wstring & FullPath);
public:
private:
	// D3D11_TEXTURE2D_DESC ReadPixels(DXGI_FORMAT InFormat, vector<Color> & OutPixels);
	// D3D11_TEXTURE2D_DESC ReadPixels(ID3D11Texture2D * InSourceTexture, const DXGI_FORMAT InFormat, vector<Color> & OutPixels);
	static HRESULT LoadTextureFromTexture
	(
		ID3D11Texture2D * InSourceTexture,
		ID3D11Texture2D ** OutTargetTexture
	);
	
private:
	ID3D11ShaderResourceView * SRV;
	DirectX::TexMetadata TexMeta;
	wstring FileName;
};


