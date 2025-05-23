#pragma once

class Texture
{
public:
	static void SaveTextureAsFile(ID3D11Texture2D * InTexture, const wstring & FileName);
public:
	explicit Texture(const wstring& FileName, bool bDefaultPath = false);
	explicit Texture(ID3D11Texture2D * InTexture, const D3D11_TEXTURE2D_DESC & Desc);
	explicit Texture(ID3D11ShaderResourceView * InSRV, const D3D11_TEXTURE2D_DESC & Desc);
	~Texture();
public:
	void BindToGPU(UINT SlotNum = 0, UINT InShaderType = (UINT)ShaderType::PixelShader) const;
	UINT GetWidth() const { return TexMeta.width; }
	UINT GetHeight() const { return TexMeta.height; }

	DXGI_FORMAT GetFormat() const { return TexMeta.format; }
	ID3D11ShaderResourceView * GetSRV() const { return SRV; }
	operator ID3D11ShaderResourceView * const *() const { return &SRV; }
	void ExtractTextureColors(vector<Color>& OutPixels) const;
	void ExtractTextureColors(vector<Color>& OutPixels, const Vector2D& VertexNum) const;
private:
	HRESULT LoadTextureAndCreateSRV(const wstring & FullPath);

private:
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


