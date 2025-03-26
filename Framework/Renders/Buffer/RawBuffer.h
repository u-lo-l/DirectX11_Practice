// ReSharper disable CppNonExplicitConversionOperator

#pragma once
#include "BufferBase.h"
#include "ComputeShaderResource.h"

class RawBuffer : public BufferBase, public ComputeShaderResource
{
public:
	explicit RawBuffer(
		void * InInputData = nullptr,
		UINT InDataSize = 0,
		UINT InOutputSize = 0
	);
	~RawBuffer() override = default;

// ~ Begin BufferBase
	operator ID3D11Buffer*() { return Buffer; }
	operator ID3D11Buffer*() const { return Buffer; }
	operator const ID3D11Buffer*() const { return Buffer; }
	operator const ID3D11Buffer*() { return Buffer; }
private:
	void BindToGPU() override {};
private:
	UINT DataSize = 0; // InputSize
	string DataName;
// ~ End BufferBase

public :
	void SetInputData( const void * InData) const;
	void GetOutputData(void * OutData) const;
	void BindInputToGPU() const;
	void BindOutputToGPU() const;
// ~Begin ComputeShaderResource
protected:
	void CreateInput() override;
	void CreateSRV() override;
	void CreateOutput() override;
	void CreateUAV() override;
	void CreateResult() override;
private:
	void * OutputData = nullptr;
	UINT OutputSize = 0;
// ~End ComputeShaderResource
};