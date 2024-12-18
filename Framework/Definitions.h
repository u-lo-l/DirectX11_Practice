#pragma once

#define SAFE_DELETE(x)\
{\
	if (x != nullptr)\
	{\
		delete (x);\
		(x) = nullptr;\
	}\
}

#define SAFE_RELEASE(x)\
{\
	if (x != nullptr)\
	{\
		(x)->Release();\
		(x) = nullptr;\
	}\
}
