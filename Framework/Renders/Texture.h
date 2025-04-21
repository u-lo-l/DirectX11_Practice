#pragma once

class Texture
{
public:
	explicit Texture(const wstring& FileName, bool bDefaultPath = false);
	~Texture();
public:
	void BindToGPU(UINT SlotNum = 0, UINT InShaderType = (UINT)ShaderType::PixelShader) const;
	UINT GetWidth() const { return TexMeta.width; }
	UINT GetHeight() const { return TexMeta.height; }
	ID3D11ShaderResourceView * GetSRV() const { return SRV; }
	operator ID3D11ShaderResourceView * const *() const { return &SRV; }
	void ExtractTextureColors(vector<Color>& OutPixels) const;
private:
	HRESULT LoadTextureAndCreateSRV(const wstring & FullPath);
public:
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


