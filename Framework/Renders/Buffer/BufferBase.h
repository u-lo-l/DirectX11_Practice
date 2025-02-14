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