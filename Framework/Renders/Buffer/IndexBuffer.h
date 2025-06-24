#pragma once
#include "BufferBase.h"

class IndexBuffer : public BufferBase
{
public:
	IndexBuffer(UINT * InData, UINT InCount);
	~IndexBuffer() override = default;

	void BindToGPU() override;
private:
};