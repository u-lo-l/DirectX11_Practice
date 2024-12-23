#pragma once

#define CHECK(x) assert((x) >= 0)
#define ASSERT(x, message) assert((x) >= 0 && message)

#define SAFE_DELETE(x)\
{\
	if (x != nullptr)\
	{\
		delete (x);\
		(x) = nullptr;\
	}\
}

#define SAFE_DELETE_ARR(x)\
{\
	if (x != nullptr)\
	{\
		delete[] (x);\
		(x) = nullptr;\
	}\
}

#define SAFE_RELEASE(x)\
{\
	if (x != nullptr)\
	{\
		x->Release();\
		(x) = nullptr;\
	}\
}
