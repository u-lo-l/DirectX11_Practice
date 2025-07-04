#pragma once
#include "ShaderBase.h"

struct ShaderEntryPoints
{
	string VSEntryPoint = "VSMain";
	string PSEntryPoint = "PSMain";
	string HSEntryPoint = "HSMain";
	string DSEntryPoint = "DSMain";
	string GSEntryPoint = "GSMain";
};

/**
 * @brief 실제 Rendering을 위한 DX11변수
 */
struct RenderPass
{
	D3D11_PRIMITIVE_TOPOLOGY Topology;

	// TODO : shared_ptr
	ID3D11InputLayout * InputLayout = nullptr;

	// TODO : unique_ptr
	ID3D11VertexShader * VertexShader = nullptr;
	// TODO : unique_ptr
	ID3D11GeometryShader * GeometryShader = nullptr;
	// TODO : unique_ptr
	ID3D11HullShader * HullShader = nullptr;
	// TODO : unique_ptr
	ID3D11DomainShader * DomainShader = nullptr;
	// TODO : unique_ptr
	ID3D11PixelShader * PixelShader = nullptr;

	map<int, SamplerStateDesc> SamplerStates;
	// TODO : shared_ptr
	ID3D11RasterizerState * RasterizerState = nullptr;
	// TODO : shared_ptr
	ID3D11BlendState * BlendState = nullptr;
	// TODO : shared_ptr
	ID3D11DepthStencilState * DepthStencilState = nullptr;
};

/**
 * \struct RenderingShaderDesc
 * \brief RenderingShader 생성을 위한 구조체.
 */
struct RenderingShaderDesc
{
	/// @brief Runtime에 사용될 Shader객체의 이름
	string ShaderName;
	/// @brief ShaderFileName : W_SHADER_PATH뒤에 오는 실제 셰이더 파일의 상대경로
	wstring ShaderFileName;
	/// @brief PreCompliedShaderFileDirectory : .cso 파일의 절대경로  
	wstring PreCompiledShaderFileDirectory;
	/// @brief InputLayoutElements : InputLayout 생성에 사용될 Data
	const vector<D3D11_INPUT_ELEMENT_DESC> * pInputLayoutElements;
	/// @brief Topology : Primitive Topology (default : TriangleList)
	D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	/// @brief ShaderMacros : 추가적인 ShaderMacro
	vector<pair<string, string>> ShaderMacros = {};
	/// @brief TargetShader : VS, DS, HS, GS, PS 중 어떤 쉐이더들이 사용될 지
	ShaderType TargetShaderType = ShaderType::VP;
	/// @brief EntryPoints : 각 쉐이더의 EntryPoint
	ShaderEntryPoints EntryPoints;
	/// @brief RasterizerStateName : ShaderManager에 등록된 RasterizerState 이름
	string RasterizerStateName;
	/// @brief DepthStencilStateName : ShaderManager에 등록된 DepthStencilState 이름
	string DepthStencilStateName;
	/// @brief BlendStateName : ShaderManager에 등록된 BlendState 이름
	string BlendStateName;
	/// @brief SamplerStateNames : ShaderManager에 등록된 SamplerState의 이름과 TargetShader
	vector<tuple<int, ShaderType, string>> SamplerStateNames;
	/// @brief bForceRecomplie :
	bool bForceRecompile = false;
};

class RenderingShader : public ShaderBase
{
public:
	static constexpr int VertexSlot = 0;
	static constexpr int InstanceSlot = 9;
public:
	explicit RenderingShader(const RenderingShaderDesc & InDesc);
	virtual ~RenderingShader() override;
	virtual void Recompile() override;
	
	ID3D11InputLayout* GetInputLayout();
	const ID3D11InputLayout* GetInputLayout() const;
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const;

	static void Draw(VertexBuffer * VB, IndexBuffer * IB, InstanceBuffer * InstB);
	static void Draw(UINT VertexCount, UINT StartVertexLocation = 0);
	static void DrawIndexed(UINT IndexCount, UINT StartIndexLocation = 0, int BaseVertexLocation = 0);
	static void DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation = 0, UINT StartInstanceLocation = 0);
	static void DrawIndexedInstanced(UINT IndexCountPreInstance, UINT InstanceCount, UINT StartIndexLocation = 0, int BaseVertexLocation = 0, UINT StartInstanceLocation = 0);

	static void ClearPass();
	void SetPass(int PassIndex = 0) const;
	void InitializeInputLayout(ID3DBlob* InVertexShaderBlob);
	bool IsDepthEnabled() const;
private:
	static bool Verify(const RenderingShaderDesc& InDesc, string& OutMessage);
	void LoadShader(ShaderType InType);
	void Recompile(ShaderType InShaderType);
	
	virtual wstring GetEntryPoint(ShaderType Type = ShaderType::None) const override;
	virtual string GetShaderTarget(ShaderType Type = ShaderType::None) const override;
	virtual ID3DBlob * CompileShader(const wstring& InFileName, const vector<pair<string, string>>& InMacros, ShaderType InType = ShaderType::None) override;
	virtual ID3DBlob * LoadPreCompiled(const wstring& InFilename) override;
	virtual HRESULT CreateShader(ID3DBlob* ShaderBlob, ShaderType InType = ShaderType::None) override;

private:
	RenderPass Pass;
	RenderingShaderDesc Desc;
};
