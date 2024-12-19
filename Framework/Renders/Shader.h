#pragma once

class Shader // NOLINT(cppcoreguidelines-special-member-functions)
{
public:
	friend struct Pass;

public:
	explicit Shader(const wstring& File);
	~Shader();

	void Draw(UINT TechniqueIndex, UINT PassIndex, UINT VertexCount, UINT StartVertexLocation = 0);
	void Dispatch(UINT TechniqueIndex, UINT PassIndex, UINT X, UINT Y, UINT Z) const;

	ID3DX11EffectVariable * Variable(const string & Name) const;
	ID3DX11EffectScalarVariable * AsScalar(const string & Name) const;
	ID3DX11EffectVectorVariable * AsVector(const string & Name) const;
	ID3DX11EffectMatrixVariable * AsMatrix(const string & Name) const;
	ID3DX11EffectStringVariable * AsString(const string & Name) const;
	ID3DX11EffectShaderResourceVariable * AsSRV(const string & Name) const;
	ID3DX11EffectRenderTargetViewVariable * AsRTV(const string & Name) const;
	ID3DX11EffectDepthStencilViewVariable * AsDSV(const string & Name) const;
	ID3DX11EffectUnorderedAccessViewVariable * AsUAV(const string & Name) const;
	ID3DX11EffectConstantBuffer * AsConstantBuffer(const string & Name) const;
	ID3DX11EffectShaderVariable * AsShader(const string & Name) const;
	ID3DX11EffectBlendVariable * AsBlend(const string & Name) const;
	ID3DX11EffectDepthStencilVariable * AsDepthStencil(const string & Name) const;
	ID3DX11EffectRasterizerVariable * AsRasterizer(const string & Name) const;
	ID3DX11EffectSamplerVariable * AsSampler(const string & Name) const;

private:
	void CreateEffect();
	static ID3D11InputLayout * CreateInputLayout(
		ID3DBlob * FxBlob,
		const D3DX11_EFFECT_SHADER_DESC * EffectVsDesc,
		const vector<D3D11_SIGNATURE_PARAMETER_DESC>& Params
	);

private:
	wstring FileName;

	ID3DX11Effect* Effect;
	D3DX11_EFFECT_DESC EffectDesc;

private:
	struct StateBlock
	{
		ID3D11RasterizerState* RSRasterizerState;

		ID3D11BlendState* OMBlendState;
		FLOAT OMBlendFactor[4];
		UINT OMSampleMask;
		ID3D11DepthStencilState* OMDepthStencilState;
		UINT OMStencilRef;
	};
	StateBlock * InitialStateBlock;

private:
	struct Pass
	{
		wstring Name;
		ID3DX11EffectPass* IPass;
		D3DX11_PASS_DESC Desc;

		ID3D11InputLayout* InputLayout;
		D3DX11_PASS_SHADER_DESC PassVsDesc;
		D3DX11_EFFECT_SHADER_DESC EffectVsDesc;
		vector<D3D11_SIGNATURE_PARAMETER_DESC> SignatureDescs;

		D3DX11_STATE_BLOCK_MASK StateBlockMask;
		StateBlock* StateBlock;

		void Draw(UINT VertexCount, UINT StartVertexLocation = 0);

		void BeginDraw();
		void EndDraw() const;

		void Dispatch(UINT X, UINT Y, UINT Z) const;
	};

	struct Technique
	{
		wstring Name;
		D3DX11_TECHNIQUE_DESC Desc;
		ID3DX11EffectTechnique* ITechnique;

		vector<Pass> Passes;

		void Draw(UINT Pass, UINT VertexCount, UINT StartVertexLocation = 0);

		void Dispatch(UINT Pass, UINT X, UINT Y, UINT Z) const;
	};
	vector<Technique> Techniques;
};