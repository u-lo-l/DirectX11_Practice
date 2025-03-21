#pragma once
#include "BufferBase.h"

class VertexBuffer : public BufferBase
{
public:
	VertexBuffer(void * InData, UINT InCount, UINT InStride, UINT InSlot = 0, bool InCpuWrite = false, bool InGpuWrite = false);
	~VertexBuffer() override = default;
	VertexBuffer(const VertexBuffer &) = delete;
	VertexBuffer(VertexBuffer &&) = delete; 
	VertexBuffer & operator=(const VertexBuffer &) = delete;
	VertexBuffer & operator=(VertexBuffer &&) = delete;
	void BindToGPU() override;
private:
	UINT Slot;

	bool bCpwWrite;
	bool bGpuWrite;
};

class InstanceBuffer final : public VertexBuffer
{
public:
	InstanceBuffer(void * InData, UINT InCount, UINT InStride);
	~InstanceBuffer() override = default;
	InstanceBuffer(const InstanceBuffer &) = delete;
	InstanceBuffer(InstanceBuffer &&) = delete; 
	InstanceBuffer & operator=(const InstanceBuffer &) = delete;
	InstanceBuffer & operator=(InstanceBuffer &&) = delete;
};