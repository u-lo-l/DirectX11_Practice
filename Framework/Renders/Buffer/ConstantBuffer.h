#pragma once
#include "BufferBase.h"

class ConstantBuffer final : public BufferBase
{
public:
	explicit ConstantBuffer(
		ShaderType TargetShaderType, 
		void * InData = nullptr,
		string InDataName = "",
		UINT InDataSize = 0,
		bool bStatic = false
	);
	~ConstantBuffer() override = default;

	operator ID3D11Buffer *() { return Buffer; }
	operator ID3D11Buffer *() const { return Buffer; }
	operator const ID3D11Buffer *() const { return Buffer; }
	operator const ID3D11Buffer *() { return Buffer; }

	void BindToGPU() override;
	ShaderType GetTargetShaderType() const { return TargetShaderType; }
private:
	UINT DataSize = 0;
	string DataName;
	bool bIsStatic = false;
	ShaderType TargetShaderType;
};