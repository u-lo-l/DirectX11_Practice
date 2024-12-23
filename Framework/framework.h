#pragma once

#include <Windows.h>
#include <assert.h>

#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
using namespace std;

/* DirectX */
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "Effects11d.lib")

/* ImGui */
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#pragma comment(lib, "imgui/IMGUI.lib")

/* My Own */
typedef D3DXCOLOR	Color;
typedef D3DXVECTOR3	Vector;
typedef D3DXVECTOR2	Vector2D;
typedef D3DXMATRIX	Matrix;
#include "Definitions.h"
#include "Systems/D3D.h"
#include "Systems/Gui.h"
#include "Systems/Keyboard.h"
#include "Renders/Shader.h"
#include "Renders/VertexLayout.h"
#include "Utilties/Math.h"
