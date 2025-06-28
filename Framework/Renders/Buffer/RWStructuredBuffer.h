#pragma once

class RWStructuredBuffer : public BufferBase
{
public:
	explicit RWStructuredBuffer(
		ShaderType TargetShaderType,
		int RegisterIndex = 0,
		void * InData = nullptr,
		UINT InCount = 0,
		UINT InStride = 0
	);
	virtual ~RWStructuredBuffer() override;
	void BindToGPUAsUAV(UINT SlotNum = 0) const;
	void BindToGPUAsSRV(UINT SlotNum = 0) const;
	void BindToGPUAsSRV(UINT SlotNum, ShaderType InShaderType) const;
	void GetResult(void * OutData) const;
	ShaderType GetTargetShaderType() const { return TargetShaderType; }

private:
	void CreateUAV();
	void CreateSRV();
	void CreateResultBuffer();
	virtual void BindToGPU() override;
	int RegisterIndex;
	UINT DataSize = 0;
	bool bIsStatic = false;
	ShaderType TargetShaderType;

	ID3D11ShaderResourceView * SRV = nullptr;
	ID3D11UnorderedAccessView * UAV = nullptr;
	ID3D11Buffer * ResultBuffer = nullptr;
};
