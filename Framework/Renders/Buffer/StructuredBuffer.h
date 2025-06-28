#pragma once

class StructuredBuffer final : public BufferBase 
{
public:
	explicit StructuredBuffer(
		ShaderType TargetShaderType,
		int RegisterIndex = 0,
		void * InData = nullptr,
		UINT InCount = 0,
		UINT InStride = 0,
		bool bStatic = false
	);
	virtual ~StructuredBuffer() override;

	virtual void BindToGPU() override;
	void UpdateData(void * InData, UINT InDataSize);
	ShaderType GetTargetShaderType() const { return TargetShaderType; }

private:
	int RegisterIndex;
	UINT DataSize = 0;
	bool bIsStatic = false;
	ShaderType TargetShaderType;

	ID3D11ShaderResourceView * SRV = nullptr;
};
