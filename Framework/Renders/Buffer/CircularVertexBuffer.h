#pragma once

class CircularVertexBuffer : public VertexBuffer
{
public:
	CircularVertexBuffer(void * InData, UINT InCount, UINT InStride, UINT InSlot = 0);
	~CircularVertexBuffer() override = default;
	CircularVertexBuffer(const CircularVertexBuffer &) = delete;
	CircularVertexBuffer(CircularVertexBuffer &&) = delete; 
	CircularVertexBuffer & operator=(const CircularVertexBuffer &) = delete;
	CircularVertexBuffer & operator=(CircularVertexBuffer &&) = delete;
	void UpdateBuffer(void * InData, UINT StartIndex, UINT InCount);
};
