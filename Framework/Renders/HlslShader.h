#pragma once

class HlslShader
{
private:
	enum class ShaderType : uint8_t
	{
		Vertex, Pixel, Compute, Max
	};
	static constexpr string GetShaderTarget(ShaderType Type);
	static constexpr string GetEntryPoint(ShaderType Type);
	constexpr static int VertexSlot = 0;
	constexpr static int InstanceSlot = 9;
public:
	HlslShader(const wstring & VsFileName, const wstring & PsFileName, const wstring & CsFileName = L"");
	~HlslShader();
private:
	HlslShader(const HlslShader&) = delete;
	HlslShader& operator=(const HlslShader&) = delete;
	HlslShader(HlslShader&&) = delete;
	HlslShader& operator=(HlslShader&&) = delete;

private:
	void CompileShader(ShaderType Type, const wstring & ShaderFileName, const);
private:
	ID3D11DeviceContext * const DeviceContext;
	ID3D11VertexShader * VertexShader;
	ID3D11PixelShader * PixelShader;
	ID3D11ComputeShader * ComputeShader;
	ID3D11InputLayout * InputLayout;
};

constexpr string HlslShader::GetShaderTarget( ShaderType Type )
{
	switch (Type)
	{
		case ShaderType::Vertex:
			return "vs_5_0";
		case ShaderType::Pixel:
			return "ps_5_0";
		case ShaderType::Compute:
			return "cs_5_0";
		default:
			return "";
	}
}

constexpr string HlslShader::GetEntryPoint( ShaderType Type )
{
	switch (Type)
	{
		case ShaderType::Vertex:
			return "VSMain";
		case ShaderType::Pixel:
			return "PSMain";
		case ShaderType::Compute:
			return "CSMain";
		default :
			return "";
	}
}
