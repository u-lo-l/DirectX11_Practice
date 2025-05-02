#pragma once

class RawBuffer;

class HlslComputeShader
{
public:
    explicit HlslComputeShader(
        const wstring & ShaderFileName,
        const D3D_SHADER_MACRO * Macros = nullptr, 
        const string & EntryPoint = "CSMain"
    );
    ~HlslComputeShader();

    void Dispatch(const RawBuffer * InRawBuffer, UINT X, UINT Y, UINT Z) const;
    void Dispatch(UINT X, UINT Y, UINT Z) const;
private:
    ID3D11ComputeShader * ComputeShader;
    wstring FileName;
};
