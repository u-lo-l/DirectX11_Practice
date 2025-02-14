#pragma once
#include "BufferBase.h"

class ComputeShaderResource
{
public:
	ComputeShaderResource() = default;
	virtual ~ComputeShaderResource();
	ComputeShaderResource(const ComputeShaderResource &) = delete;
	ComputeShaderResource(ComputeShaderResource &&) = delete; 
	ComputeShaderResource & operator=(const ComputeShaderResource &) = delete;
	ComputeShaderResource & operator=(ComputeShaderResource &&) = delete;

public:
	operator ID3D11UnorderedAccessView* () { return UAV; }
	operator ID3D11UnorderedAccessView* () const { return UAV; }
	operator const ID3D11UnorderedAccessView* () { return UAV; }
	operator const ID3D11UnorderedAccessView* () const { return UAV; }
	
protected:
	virtual void CreateInput() = 0;
	virtual void CreateSRV() = 0;
	virtual void CreateOutput() = 0;
	virtual void CreateUAV() = 0;
	virtual void CreateResult() = 0;

	void CreateBuffer();
protected:
	ID3D11Resource * Input = nullptr; // GPU input
	ID3D11ShaderResourceView * SRV = nullptr;

	ID3D11Resource * Output = nullptr; // from GPU to CPU output
	ID3D11UnorderedAccessView * UAV = nullptr;
	
	ID3D11Resource * Result = nullptr;
};

class RawBuffer : public BufferBase, public ComputeShaderResource
{
public:
	explicit RawBuffer(
		void * InData = nullptr,
		string InDataName = "",
		UINT InDataSize = 0,
		UINT InOutputSize = 0
	);
	~RawBuffer() override = default;

// ~ Begin BufferBase
	operator ID3D11Buffer *() { return Buffer; }
	operator ID3D11Buffer *() const { return Buffer; }
	operator const ID3D11Buffer *() const { return Buffer; }
	operator const ID3D11Buffer *() { return Buffer; }
	
	void BindToGPU() override {};
private:
	UINT DataSize = 0; // InputSize
	string DataName;
// ~ End BufferBase

public :
	void SetInputData( const void * InData);
	void GetOutputData(void * OutData);
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