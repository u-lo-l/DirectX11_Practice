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

    // Sampler
    HRESULT CreateSamplerState_Linear_Clamp();
    HRESULT CreateSamplerState_Linear_Border();
    HRESULT CreateSamplerState_Anisotropic();
    HRESULT CreateSamplerState_Anisotropic_Clamp();
    HRESULT CreateSamplerState_Anisotropic_Wrap();
    HRESULT CreateSamplerState_ShadowSampler();
    HRESULT CreateSamplerState_Linear_Wrap();
    HRESULT CreateSamplerState(
        const D3D11_SAMPLER_DESC * SampDesc,
        SamplerStateType SamplerType = SamplerStateType::Linear
    );
private:
    ID3D11ComputeShader * ComputeShader;
    wstring FileName;
    UINT DispatchSize[3] = {1, 1, 1};
    array<ID3D11SamplerState *, static_cast<UINT>(SamplerStateType::Max)> SamplerStates;
};
