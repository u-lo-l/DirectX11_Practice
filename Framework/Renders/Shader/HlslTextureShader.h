#pragma once

class HlslTextureShader
{
private:
	constexpr static int Const_ViewProjection_VS = 1;
	constexpr static int Texture_Texture_PS = 0;
	constexpr static int Sampler_Linear_PS = 0;
	using VertexType = VertexTexture;
public:
	explicit HlslTextureShader(ID3D11ShaderResourceView* InSRV);
	~HlslTextureShader();

	void Tick();
	void Render();

	Transform * GetTransform() const { return World; }

private:
	HlslShader<VertexTexture> * Shader;
	
	struct Desc
	{
		Matrix View;
		Matrix Projection;
	} Data;
	ConstantBuffer * CBuffer;
	
	Transform* World;
	VertexBuffer* VBuffer;
	ID3D11ShaderResourceView * SRV;
};
