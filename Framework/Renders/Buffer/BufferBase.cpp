#include "framework.h"
#include "BufferBase.h"

BufferBase::~BufferBase()
{
#ifdef DO_DEBUG
	printf("%s : Buffer Safely Released : [%s]\n", BufferType.c_str(), BufferInfo.c_str());
#endif
	SAFE_RELEASE(Buffer);
}
