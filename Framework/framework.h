#pragma once

#include <Windows.h>
#include <assert.h>

#include <string>
#include <vector>
#include <algorithm>
using namespace std;


#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "Effects11d.lib")

typedef D3DXCOLOR Color;
typedef D3DXVECTOR3 Vector;

#include "Definitions.h"

#include "Systems/D3D.h"

#include "Renders/Shader.h"
#include "Renders/Shader.h"