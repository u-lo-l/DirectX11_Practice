// ReSharper disable CppNonExplicitConversionOperator
// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
#pragma once

class BufferBase
{
protected:
	// BufferBase(void * InData, UINT InCount, UINT InStride);
	virtual ~BufferBase();
public:
	virtual void BindToGPU() = 0;
protected:
	ID3D11Buffer * Buffer = nullptr;
	void * Data = nullptr;
	UINT Count = 0;
	UINT Stride = 0;
#ifdef DO_DEBUG
	string BufferInfo;
	string BufferType;
#endif
};

class VertexBuffer : public BufferBase
{
public:
#ifdef DO_DEBUG
	VertexBuffer( void * InData,
					UINT InCount,
					UINT InStride,
					const string & MetaData = "",
					UINT InSlot = 0,
					bool InCpuWrite = false,
					bool InGpuWrite = false
				);
#else
	VertexBuffer(void * InData, UINT InCount, UINT InStride, UINT InSlot = 0, bool InCpuWrite = false, bool InGpuWrite = false);
#endif
	~VertexBuffer() override = default;
	void BindToGPU() override;
private:
	UINT Slot;

	bool bCpwWrite;
	bool bGpuWrite;
};

/*=============================================================================*/

class IndexBuffer : public BufferBase
{
public:
	IndexBuffer(UINT * InData, UINT InCount);
	~IndexBuffer() override = default;

	UINT GetCount() const { return Count; }
	void BindToGPU() override;
private:
};

/*=============================================================================*/

class ConstantBuffer : public BufferBase
{
public:
	explicit ConstantBuffer(void * InData = nullptr, string InDataName = "", UINT InDataSize = 0);
	~ConstantBuffer() override = default;

	operator ID3D11Buffer *() { return Buffer; }
	operator ID3D11Buffer *() const { return Buffer; }
	operator const ID3D11Buffer *() const { return Buffer; }
	operator const ID3D11Buffer *() { return Buffer; }
	
	void BindToGPU() override;
private:
	UINT DataSize = 0;
	string DataName;
};

/*=============================================================================*/

/*
 * RAM에서 VRAM으로 Input이 넘어가고 GPU가 ComputeShade를 수행한다.
 * 그리고 Output이 나온다. 그런데 이 Output은 ReadOnly라서 사본을 만들어야한다.
 *
 * Input는 SRV로 넘어간다. (Resource를 Shader에 넘겨주는 방식이다.)
 * Output은 UAV(Unordered Access View)
 * - SRV : Shader에서 읽을 수 있는 자원을 지정하는 view
 * - UAV : 자원을 읽고 쓰기 위한 view
 *
 * Result :
 *  - Usage : Staging
 *
 *  다시~
 *  RAM에서 vRAM으로 데이터를 넘길 때 GPU가 읽을 수 있게 하는 것은 SRV이다.
 *	SRV는 GPU가 읽기만 가능하지만, UAV는 읽기와 쓰기 모두 가능하다.
 *	그래서 Compute Shader의 결과를 UAV에 쓴다.
 *
 *	
 */
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
	ID3D11Resource * Input = nullptr; // GPU의 input으로 들어가는 Resource
	ID3D11ShaderResourceView * SRV = nullptr;

	ID3D11Resource * Output = nullptr; // GPU에서 CPU로 나오는 Resource
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