#pragma once

/*
 * RWTexture2D 
 */
class RWTexture2D
{
public:
	RWTexture2D(UINT Width, UINT Height, DXGI_FORMAT InTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM);
	~RWTexture2D();
	/*
	 * PixelShader에서 사용될 수도 있지만, ComputeShader에서만 사용한다고 가정.
	 * PixelShader는 rWTexture에 쓰기 보다 RenderTarget에 그리자.
	 */
	void BindToGPUAsUAV(UINT SlotNum = 0) const;
	void BindToGPUAsSRV(UINT SlotNum = 0) const;
	void BindToGPUAsSRV(UINT SlotNum, UINT InShaderType) const;
	void UpdateSRV();
	UINT GetWidth() const;
	UINT GetHeight() const;
	ID3D11UnorderedAccessView * GetUAV() const;
	ID3D11ShaderResourceView * GetSRV() const;
	void SaveOutputAsFile(const wstring & FileName) const;
	void ExtractTextureColors(vector<Color>& OutPixels, const Vector2D& VertexNum) const;

private:
	void CreateOutputTextureAndUAV(); // GPU Only
	void CreateResultTexture(); // for CPU Write
	UINT Width = 0;
	UINT Height = 0;
	DXGI_FORMAT TextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	ID3D11Texture2D * OutputTexture = nullptr;
	ID3D11UnorderedAccessView * UAV = nullptr;
	ID3D11ShaderResourceView  * SRV = nullptr;
	ID3D11Texture2D * ResultTexture = nullptr;
};
