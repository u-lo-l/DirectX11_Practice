#pragma once


class BufferBase
{
protected:
	virtual ~BufferBase();
public:
	virtual void BindToGPU() = 0;
protected:
	ID3D11Buffer * Buffer = nullptr;
	void * Data = nullptr;
	UINT Count = 0;
	UINT Stride = 0;
};