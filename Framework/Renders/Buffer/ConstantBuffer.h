#pragma once
#include "BufferBase.h"

class ConstantBuffer final : public BufferBase
{
public:
	explicit ConstantBuffer(
		UINT TargetShaderType,
		int RegisterIndex = 0,
		void * InData = nullptr,
		string InDataName = "",
		UINT InDataSize = 0,
		bool bStatic = false
	);
	explicit ConstantBuffer(
		ShaderType TargetShaderType,
		int RegisterIndex = 0,
		void * InData = nullptr,
		const string& InDataName = "",
		UINT InDataSize = 0,
		bool bStatic = false
	);
	explicit ConstantBuffer(
		ShaderType TargetShaderType,
		int RegisterIndex = 0,
		void * InData = nullptr,
		UINT InDataSize = 0,
		bool bStatic = false
	);
	virtual ~ConstantBuffer() override;

	void UpdateData(void * InData = nullptr, UINT InDataSize = 0);
	virtual void BindToGPU() override;
	void BindToGPU(int InRegisterIndex) const;
	void BindToGPU(ShaderType TargetShader, int InRegisterIndex) const;
	UINT GetTargetShaderType() const { return TargetShaderType; }
private:
	int RegisterIndex;
	UINT DataSize = 0;
	string DataName;
	bool bIsStatic = false;
	UINT TargetShaderType;
};
