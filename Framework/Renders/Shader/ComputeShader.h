#pragma once
#include "ShaderBase.h"

struct DispatchPass
{
	// TODO : unique_ptr
	ID3D11ComputeShader * Shader = nullptr;
	map<int, SamplerStateDesc> SamplerStates;
	unsigned int DispatchX = 1;
	unsigned int DispatchY = 1;
	unsigned int DispatchZ = 1;
};

struct ComputeShaderDesc
{
	string ShaderName;
	wstring ShaderFileName;
	wstring PreCompiledShaderFileDirectory;
	const D3D_SHADER_MACRO * ShaderMacros = nullptr;
	wstring EntryPoint = L"CSMain";
	unsigned int DispatchX = 1;
	unsigned int DispatchY = 1;
	unsigned int DispatchZ = 1;
	vector<tuple<int, ShaderType, string>> SamplerStateNames;
	bool bForceRecompile = false;
};

class ComputeShader : public ShaderBase
{
public:
	explicit ComputeShader(const ComputeShaderDesc & InDesc);
	virtual ~ComputeShader() override;
	void Dispatch() const;

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
