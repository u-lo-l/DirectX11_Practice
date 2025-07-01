#pragma once
#include <assert.h>

#define CHECK(x) assert((x) == true)
#define ASSERT(x, message) \
	if (!(x)) { \
		string Msg = String::Format("%s | %s", __FUNCTION__, (message));\
		MessageBoxA(NULL, Msg.c_str(), "Assertion Failed", MB_ICONERROR | MB_OK); \
		assert(x); \
	}

#define SAFE_DELETE(x)\
{\
	if ((x) != nullptr)\
	{\
		delete (x);\
		(x) = nullptr;\
	}\
}

#define SAFE_DELETE_ARR(x)\
{\
	if ((x) != nullptr)\
	{\
		delete[] (x);\
		(x) = nullptr;\
	}\
}

#define SAFE_RELEASE(x)\
{\
	if ((x) != nullptr)\
	{\
		(x)->Release();\
		(x) = nullptr;\
	}\
}

#define SHADER_PATH		"../_Shaders/"
#define MODEL_PATH		"../_Models/"
#define ANIMATION_PATH	"../_Animations/"
#define MATERIAL_PATH	"../_Materials/"
#define TEXTURE_PATH	"../_Textures/"
#define ASSET_PATH		"../_Assets/"

#define W_SHADER_PATH		L"../_Shaders/"
#define W_MODEL_PATH		L"../_Models/"
#define W_ANIMATION_PATH	L"../_Animations/"
#define W_MATERIAL_PATH		L"../_Materials/"
#define W_TEXTURE_PATH		L"../_Textures/"
#define W_ASSET_PATH		L"../_Assets/"
