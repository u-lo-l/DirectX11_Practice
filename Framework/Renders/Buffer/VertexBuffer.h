#pragma once
#include "BufferBase.h"

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