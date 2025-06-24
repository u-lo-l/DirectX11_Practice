#pragma once

class RWStructuredBuffer : public BufferBase
{
public:
	explicit RWStructuredBuffer(
		UINT TargetShaderType,
		int RegisterIndex = 0,
		void * InData = nullptr,
		UINT InCount = 0,
		UINT InStride = 0
	);
	~RWStructuredBuffer() override;
	void BindToGPUAsUAV(UINT SlotNum = 0) const;
	void BindToGPUAsSRV(UINT SlotNum = 0) const;
	void BindToGPUAsSRV(UINT SlotNum, ShaderType InShaderType) const;
	void UpdateSRV();
	void GetResult(void * OutData) const;
	UINT GetTargetShaderType() const { return TargetShaderType; }

private:
	void CreateUAV();
	void CreateResultBuffer();
	void BindToGPU() override;
	int RegisterIndex;
	UINT DataSize = 0;
	bool bIsStatic = false;
	UINT TargetShaderType;

	ID3D11ShaderResourceView * SRV = nullptr;
	ID3D11UnorderedAccessView * UAV = nullptr;
	ID3D11Buffer * ResultBuffer = nullptr;
};
