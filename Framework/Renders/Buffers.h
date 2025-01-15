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
};

class VertexBuffer : public BufferBase
{
public:
	VertexBuffer(void * InData, UINT InCount, UINT InStride, UINT InSlot = 0, bool InCpuWrite = false, bool InGpuWrite = false);
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
	ConstantBuffer(void * InData = nullptr, string InDataName = "", UINT InDataSize = 0);
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