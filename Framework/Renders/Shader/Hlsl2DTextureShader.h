#pragma once

class Hlsl2DTextureShader
{
private:
	constexpr static int Const_ViewProjection_VS = 1;
	constexpr static int Texture_Texture_PS = 0;
	constexpr static int Sampler_Linear_PS = 0;
	using VertexType = VertexTexture;
public:
	explicit Hlsl2DTextureShader(ID3D11ShaderResourceView* InSRV);
	~Hlsl2DTextureShader();

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
