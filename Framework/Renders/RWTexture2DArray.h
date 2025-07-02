#pragma once
#include "Shader/ComputeShader.h"

class RWTexture2DArray
{
public:

	explicit RWTexture2DArray(
		UINT InCount,
		UINT InWidth = 512,
		UINT InHeight = 512,
		DXGI_FORMAT InFormat = DXGI_FORMAT_R8G8B8A8_UNORM
	);
	~RWTexture2DArray();
	// void BindToGPUAsUAV(UINT SlotNum = 0) const;
	void BindToGPUAsSRV(UINT SlotNum = 0, ShaderType InShaderType = ShaderType::ComputeShader) const;
	UINT GetWidth() const;
	UINT GetHeight() const;
	ID3D11UnorderedAccessView * GetUAV() const;
	ID3D11ShaderResourceView  * GetSRV() const;
	void GetSRV(UINT InSlice, ID3D11ShaderResourceView** OutSRV) const;
	void SaveOutputAsFile(const wstring & InFileName) const;
private:
	void CreateOutputTexture();
	void CreatUAV();
	void CreatSRV();
	void CreateResultTexture();
	
	ID3D11UnorderedAccessView * UAV;
	ID3D11ShaderResourceView  * SRV;
	UINT ArraySize;
	UINT Width;
	UINT Height;
	DXGI_FORMAT TextureFormat;
	ID3D11Texture2D * OutputTextureArray;
	ID3D11Texture2D * ResultTextureArray;
};
