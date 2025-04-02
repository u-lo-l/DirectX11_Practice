#pragma once

class PostEffect
{
	constexpr static int Slot_Const_PixelSize_PS = 0;
	using VertexType = Vertex;
public:
	PostEffect(const wstring & InShaderFileName, ID3D11ShaderResourceView * InSRV);
	~PostEffect();

	void Tick();
	void Render();

private:
	HlslShader<VertexType> * Shader;
	ID3D11ShaderResourceView * SRV;
	VertexBuffer * VBuffer;

	float PostEffectFactors[8];
	ConstantBuffer * PostEffectFactors_CBuffer_PS = nullptr;
};
