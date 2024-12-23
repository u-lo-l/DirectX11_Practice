#pragma once

/// <summary>
/// Effect : Dx Effect는 HLSL로 쓰인 파이프라인 State의 집합이다.(?)
/// </summary>
/// <remarks>
/// - Pipeline State : 그래픽스 파이프라인의 구성요소.
/// Render Target, Shader Program이 이에 해당된다.
/// - Effect Framework는 PipelineState를 설정하는 프레임워크이다.
/// </remarks>
/// <remarks>
/// <br>
/// <br>
/// 우선 Effect에서 제어하려는 State를 구성해야한다.
/// State <- vertices, hull, domain, shader, texture 등
/// Effect는 메모리내에서 literal 문자열로 써도 되지만, 별도의 파일(.fx파일)로 작성하는 것이 좋다.
///
/// Effect는 Render State를 하나의 Technique로 캡슐화한다.
/// Technique는 렌더링 함수이다.
/// <c>
/// technique11 T0
/// {
///		pass P0
///		{
///			SetVertexShader(CompileShader(vs_5_0, VS()));
///			SetPixelShader(CompileShader(ps_5_0, PS()));
///		}
///		pass P1
///		{
///			SetRasterizerState(FillMode_WireFrame);
///			SetVertexShader(CompileShader(vs_5_0, VS()));
///			SetPixelShader(CompileShader(ps_5_0, PS2()));
///		}
/// }
/// </c>
/// 이런 식으로 쓰인다.
/// 여러 Technique들을 그룹화 하여 fxgroup을 만들어 사용하는 것도 가능하다.
/// </remarks>
class Shader // NOLINT(cppcoreguidelines-special-member-functions)
{
public:
	friend struct Pass;

public:
	explicit Shader(const wstring& File);
	~Shader();

	void Draw(UINT TechniqueIndex, UINT PassIndex, UINT VertexCount, UINT StartVertexLocation = 0);
	void Dispatch(UINT TechniqueIndex, UINT PassIndex, UINT X, UINT Y, UINT Z) const;

	//ID3DX11EffectVariable * Variable(const string & Name) const;
	//ID3DX11EffectScalarVariable * AsScalar(const string & Name) const;
	//ID3DX11EffectVectorVariable * AsVector(const string & Name) const;
	//ID3DX11EffectMatrixVariable * AsMatrix(const string & Name) const;
	//ID3DX11EffectStringVariable * AsString(const string & Name) const;
	//ID3DX11EffectShaderResourceVariable * AsSRV(const string & Name) const;
	//ID3DX11EffectRenderTargetViewVariable * AsRTV(const string & Name) const;
	//ID3DX11EffectDepthStencilViewVariable * AsDSV(const string & Name) const;
	//ID3DX11EffectUnorderedAccessViewVariable * AsUAV(const string & Name) const;
	//ID3DX11EffectConstantBuffer * AsConstantBuffer(const string & Name) const;
	//ID3DX11EffectShaderVariable * AsShader(const string & Name) const;
	//ID3DX11EffectBlendVariable * AsBlend(const string & Name) const;
	//ID3DX11EffectDepthStencilVariable * AsDepthStencil(const string & Name) const;
	//ID3DX11EffectRasterizerVariable * AsRasterizer(const string & Name) const;
	//ID3DX11EffectSamplerVariable * AsSampler(const string & Name) const;

private:
	void CreateEffect();
	/// <summary>
	/// 
	/// </summary>
	/// <param name="FxBlob"> 컴파일된 쉐이더 바이너리</param>
	/// <param name="EffectVsDesc"> VertexShader의 메타데이터</param>
	/// <param name="Params"> shader의 입력 signature들</param>
	/// <remarks>
	/// InputLayout : DX11에서 Vertex데이터를 VertexShader로 전달하는 방법을 정의하는 객체
	/// 이렇게 생성된 InputLayout은 IA단계에서 Vertices를 처리할 때 사용된다.
	/// </remarks>
	/// <returns></returns>
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

	struct Pass
	{
		//wstring Name;
		ID3DX11EffectPass* IPass = nullptr;
		D3DX11_PASS_DESC Desc;

		ID3D11InputLayout* InputLayout = nullptr;
		D3DX11_PASS_SHADER_DESC PassVsDesc;
		D3DX11_EFFECT_SHADER_DESC EffectVsDesc;
		vector<D3D11_SIGNATURE_PARAMETER_DESC> SignatureDescs;

		D3DX11_STATE_BLOCK_MASK StateBlockMask;
		StateBlock* StateBlock;

		void Draw(UINT VertexCount, UINT StartVertexLocation = 0);
		void BeginDraw();
		void EndDraw() const;
		void Dispatch(UINT X, UINT Y, UINT Z) const;
		void CheckPassValid();
		
	};

	struct Technique
	{
		//wstring Name;
		D3DX11_TECHNIQUE_DESC Desc;
		ID3DX11EffectTechnique* ITechnique = nullptr;

		vector<Pass> Passes;

		void Draw(UINT Pass, UINT VertexCount, UINT StartVertexLocation = 0);

		void Dispatch(UINT Pass, UINT X, UINT Y, UINT Z) const;
	};
	vector<Technique> Techniques;
};