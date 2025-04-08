#pragma once

class PostEffect
{
	constexpr static int Slot_Const_PixelSize_PS = 0;
	using VertexType = Vertex;
public:
	PostEffect(const wstring & InShaderFileName, RenderTarget * InRenderTarget);
	~PostEffect();

	void Tick();
	void PreRender(ID3D11ShaderResourceView * InDiffuseSRV = nullptr);
	void Render();
	void PostRender();

private:
	RenderTarget * PreRenderedTarget;
	
	void PreRender_Bloom(ID3D11ShaderResourceView * InOriginalTextureSRV);
	void PreRender_GaussianBlur(ID3D11ShaderResourceView * InOriginalTextureSRV);
	
	HlslShader<VertexType> * Shader;
	
	HlslShader<VertexType> * VertexShader;
	HlslShader<VertexType> * GaussianBlur_Shader[2];
	HlslShader<VertexType> * Bloom_Shader[2];
	VertexBuffer * VBuffer;

	float PostEffectFactors[8];
	ConstantBuffer * PostEffectFactors_CBuffer_PS = nullptr;

	float GaussianBlurFactor[6];
	ConstantBuffer * GaussianBlurFactor_CBuffer_PS = nullptr;

	enum RenderTargetType
	{
		Luminosity,
		GaussianBlur_X,
		GaussianBlur_Y,
		Final,
	};
	
	RenderTarget * Bloom_RT[4] = {nullptr, };
	DepthStencil * Bloom_DS = nullptr;
	UINT SamplingCount = 15;
	vector<float> WeightsX;
	vector<float> WeightsY;
};
