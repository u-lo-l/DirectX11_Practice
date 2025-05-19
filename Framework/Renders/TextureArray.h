#pragma once

class TextureArray
{
public:
	explicit TextureArray(
		const vector<wstring>& InNames,
		UINT InWidth = 256,
		UINT InHeight = 256,
		UINT InMipLevels = 1
	);
	~TextureArray();
	void BindToGPU(UINT SlotNum = 0) const;

private:
	static vector<ID3D11Texture2D*> CreateTextures(const vector<wstring>& InNames, UINT InWidth, UINT InHeight, UINT InMipLevels);

private:
	ID3D11ShaderResourceView* SRV;
};