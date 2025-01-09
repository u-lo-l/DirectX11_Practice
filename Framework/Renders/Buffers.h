// ReSharper disable CppNonExplicitConversionOperator
// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
#pragma once

class VertexBuffer
{
public:
	VertexBuffer(void * InData, UINT InCount, UINT InStride, UINT InSlot = 0, bool InCpuWrite = false, bool InGpuWrite = false);
	~VertexBuffer();

	void BindToGPU() const;
private:
	ID3D11Buffer * Buffer;
	void * Data;
	UINT Count;
	UINT Stride;
	UINT Slot;

	bool bCpwWrite;
	bool bGpuWrite;
};

/*=============================================================================*/

class IndexBuffer
{
public:
	IndexBuffer(UINT * InData, UINT InCount);
	~IndexBuffer();

	UINT GetCount() const { return Count; }
	void BindToGPU() const;
private:
	ID3D11Buffer * Buffer;
	UINT * Data;
	UINT Count;
};

/*=============================================================================*/

class ConstantBuffer
{
public:
	ConstantBuffer(void * InData, UINT InDataSize);
	~ConstantBuffer();

	operator ID3D11Buffer *() { return Buffer; }
	operator ID3D11Buffer *() const { return Buffer; }
	operator const ID3D11Buffer *() const { return Buffer; }
	operator const ID3D11Buffer *() { return Buffer; }
	
	void BindToGPU(const Shader * InShader = nullptr) const;
private:
	ID3D11Buffer * Buffer;
	
	void * Data;
	UINT DataSize;
};