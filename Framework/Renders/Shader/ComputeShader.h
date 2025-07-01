#pragma once
#include "ShaderBase.h"

struct DispatchPass
{
	// TODO : unique_ptr
	ID3D11ComputeShader * Shader = nullptr;
	map<int, SamplerStateDesc> SamplerStates;
	UINT DispatchX = 0;
	UINT DispatchY = 0;
	UINT DispatchZ = 0;
};

/**
 * - string ShaderName;
 * - wstring ShaderFileName;
 * - wstring PreCompiledShaderFileDirectory;
 * - const D3D_SHADER_MACRO * ShaderMacros = nullptr;
 * - wstring EntryPoint = L"CSMain";
 * - UINT NumThreadDimX = 1;
 * - UINT NumThreadDimY = 1;
 * - UINT NumThreadDimZ = 1;
 * - UINT DispatchX = 0;
 * - UINT DispatchY = 0;
 * - UINT DispatchZ = 0;
 * - vector<tuple<int, ShaderType, string>> SamplerStateNames;
 * - bool bForceRecompile = false;
 */
struct ComputeShaderDesc
{
	string ShaderName;
	wstring ShaderFileName;
	wstring PreCompiledShaderFileDirectory;
	const D3D_SHADER_MACRO * ShaderMacros = nullptr;
	wstring EntryPoint = L"CSMain";
	UINT NumThreadDimX = 1;
	UINT NumThreadDimY = 1;
	UINT NumThreadDimZ = 1;
	UINT DispatchX = 0;
	UINT DispatchY = 0;
	UINT DispatchZ = 0;
	vector<tuple<int, ShaderType, string>> SamplerStateNames;
	bool bForceRecompile = false;
};

class ComputeShader : public ShaderBase
{
public:
	explicit ComputeShader(const ComputeShaderDesc & InDesc);
	virtual ~ComputeShader() override;
	void SetPass() const;
	void GetThreadDim(UINT & X, UINT & Y, UINT & Z) const;
	static void ClearPass();
	void Dispatch() const;
	void Dispatch(UINT X, UINT Y, UINT Z) const;

private:
	void LoadShader();
	virtual wstring GetEntryPoint(ShaderType Type = ShaderType::None) const override;
	virtual string GetShaderTarget(ShaderType Type = ShaderType::None) const override;
	virtual ID3DBlob * CompileShader(const wstring& InFileName, const D3D_SHADER_MACRO* InMacros = nullptr, ShaderType InType = ShaderType::None) override;
	virtual ID3DBlob * LoadPreCompiled(const wstring& InFilename) override;
	virtual HRESULT CreateShader(ID3DBlob* ShaderBlob, ShaderType InType = ShaderType::None) override;

	DispatchPass Pass;
	ComputeShaderDesc Desc;
};
