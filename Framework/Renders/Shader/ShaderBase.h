#pragma once

class ShaderBase
{
public:
	virtual ~ShaderBase() = 0;
protected:
	virtual wstring GetEntryPoint(ShaderType Type = ShaderType::None) const = 0;
	virtual string GetShaderTarget(ShaderType Type = ShaderType::None) const = 0;

	virtual ID3DBlob * CompileShader(const wstring& InFileName, const D3D_SHADER_MACRO* InMacros, ShaderType InType = ShaderType::None) = 0;
	virtual ID3DBlob * LoadPreCompiled(const wstring& InFilename) = 0;
	virtual HRESULT CreateShader(ID3DBlob* ShaderBlob, ShaderType InType = ShaderType::None) = 0;
};
