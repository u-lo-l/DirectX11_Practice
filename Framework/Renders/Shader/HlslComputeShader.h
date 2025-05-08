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

    void SetDispatchSize(UINT X, UINT Y, UINT Z);
    void Dispatch(const RawBuffer * InRawBuffer, UINT X, UINT Y, UINT Z) const;
    void Dispatch(UINT X, UINT Y, UINT Z) const;
    void Dispatch() const;
private:
    ID3D11ComputeShader * ComputeShader;
    wstring FileName;
    UINT DispatchSize[3] = {1, 1, 1};
};
