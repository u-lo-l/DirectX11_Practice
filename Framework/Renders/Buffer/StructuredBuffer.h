#pragma once

class StructuredBuffer final : public BufferBase 
{
public:
	explicit StructuredBuffer(
		UINT TargetShaderType,
		int RegisterIndex = 0,
		void * InData = nullptr,
		UINT InCount = 0,
		UINT InStride = 0,
		bool bStatic = false
	);
	~StructuredBuffer() override;

	void BindToGPU() override;
	void UpdateData(void * InData, UINT InDataSize);
	UINT GetTargetShaderType() const { return TargetShaderType; }

private:
	int RegisterIndex;
	UINT DataSize = 0;
	bool bIsStatic = false;
	UINT TargetShaderType;

	ID3D11ShaderResourceView * SRV = nullptr;
};
