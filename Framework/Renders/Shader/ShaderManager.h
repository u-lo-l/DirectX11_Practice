#pragma once
#include <unordered_map>

class ComputeShader;
class RenderingShader;
class ShaderBase;

class ShaderManager
{
public:
	static constexpr int PerFrameBindSlot = 0;
	static constexpr int PerMaterialBindSlot = 1;
	static void Create();
	static void Destroy();
	static ShaderManager * Get();
	RenderingShader * GetRenderShader(const string& InName);
	ComputeShader* GetComputeShader(const string& InName);
	void AddShader(const wstring & InName, ShaderBase * InShader);

	void AddSamplerState(const string & InName, ID3D11SamplerState *InSamplerState);
	void AddRasterizerState(const string & InName, ID3D11RasterizerState *InRasterState);
	void AddDepthStencilState(const string & InName, ID3D11DepthStencilState *InDepthStencilState);
	void AddBlendState(const string & InName, ID3D11BlendState *InBlendState);

	ID3D11BlendState* GetBlendState(const string & InName);
	ID3D11RasterizerState* GetRasterizerState(const string & InName);
	ID3D11SamplerState* GetSamplerState(const string & InName);
	ID3D11DepthStencilState* GetDepthStencilState(const string & InName);

	vector<string> GetBlendStateNames() const;
	vector<string> GetRasterizerStateNames() const;
	vector<string> GetDepthStencilStateNames() const;
	vector<string> GetSamplerStateNames() const;
	vector<string> GetRenderingShaderNames() const;
	vector<string> GetComputeShaderNames() const;
	bool IsValidRenderingShaderName(const string & InName);
private:
	static ShaderManager * Instance;

private:
	ShaderManager();
	~ShaderManager();
	
	void InitSamplerStates();
	void InitDepthStencilStates();
	void InitBlendStates();
	void InitRasterizerStates();
	void InitRenderingShaders();
	void InitComputeShaders();
	
	void AddRenderShader(const wstring & InName, RenderingShader * InShader);
	void AddComputeShader(const wstring & InName, ComputeShader * InShader);
	
	unordered_map<string, RenderingShader *> RenderShaderMap;
	unordered_map<string, ComputeShader *> ComputeShaderMap;
	/**
	 - Point_Wrap
	 - Point_Clamp
	 - Linear_Wrap
	 - Linear_Clamp
	 - Anisotropic_Wrap
	 - Anisotropic_Clamp
	 */
	unordered_map<string, ID3D11SamplerState *> SamplerStateTable;

	/**
	 - Default
	 - NoDepth
	 - Particle
	 */
	unordered_map<string, ID3D11DepthStencilState *> DepthStencilStateTable;

	/**
	 - Opaque
	 - Additive
	 - Multiply
	 - Multiply2X
	 - AlphaBlendCoverage
	 */
	unordered_map<string, ID3D11BlendState *> BlendStateTable;

	/**
	 -WireFrame
	 -WireFrame_CullFront
	 -WireFrame_NoCull
	 *
	 -Solid
	 -Solid_CW_CullBack
	 -Solid_CullFront
	 -Solid_NoCull
	 */
	unordered_map<string, ID3D11RasterizerState *> RasterizerStateTable;
};
