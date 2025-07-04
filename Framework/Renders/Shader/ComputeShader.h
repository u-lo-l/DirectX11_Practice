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
	ComputeShaderDesc() {};
	explicit  ComputeShaderDesc(const ComputeShaderDesc& InDesc);
	ComputeShaderDesc(
		string InShaderName,
		wstring InShaderFileName,
		const vector<pair<string, string>> & InShaderMacros,
		wstring InEntryPoint,
		UINT InNumThreadDimX,
		UINT InNumThreadDimY,
		UINT InNumThreadDimZ,
		UINT InDispatchX,
		UINT InDispatchY,
		UINT InDispatchZ,
		const vector<tuple<int, ShaderType, string>>& InSamplerStateNames,
		bool InbForceRecompile
	);
	string ShaderName;
	wstring ShaderFileName;
	wstring PreCompiledShaderFileDirectory;
	vector<pair<string, string>> ShaderMacros = {};
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
	void Recompile();
	void SetPass() const;
	void GetThreadDim(UINT & X, UINT & Y, UINT & Z) const;
	void ClearPass();
	void Dispatch();
	void Dispatch(UINT X, UINT Y, UINT Z);
	void BindUAV(ID3D11UnorderedAccessView* InUAV, UINT SlotNum);
	void BindSRV(ID3D11ShaderResourceView* InSRV, UINT SlotNum);
	void BindCB(const BufferBase* InBuffer, UINT SlotNum);
	void BindCB(ID3D11Buffer* InConstantBuffer, UINT SlotNum);
	
private:
	void LoadShader();
	virtual wstring GetEntryPoint(ShaderType Type = ShaderType::None) const override;
	virtual string GetShaderTarget(ShaderType Type = ShaderType::None) const override;
	virtual ID3DBlob * CompileShader(const wstring& InFileName, const vector<pair<string, string>>& InMacros = {}, ShaderType InType = ShaderType::None) override;
	virtual ID3DBlob * LoadPreCompiled(const wstring& InFilename) override;
	virtual HRESULT CreateShader(ID3DBlob* ShaderBlob, ShaderType InType = ShaderType::None) override;

	DispatchPass Pass;
	ComputeShaderDesc Desc;
	map<UINT, ID3D11Buffer*> CBs = {};
	map<UINT, ID3D11ShaderResourceView*> SRVs = {};
	map<UINT, ID3D11UnorderedAccessView*> UAVs = {};
};
