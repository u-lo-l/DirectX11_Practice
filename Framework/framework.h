#pragma once

#include <Windows.h>
#include <assert.h>

#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
#include <chrono> // for timer, steady_clock
using namespace std;

/* DirectX */
#include <d3dcommon.h>
#include <d3d11.h>
#include <D3DX11tex.h>
#include <d3dx10math.h>
#include <d3dcompiler.h>

// #include <d3dx11effect.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

/* Effect */
#include "effect/d3dx11effect.h"
#pragma comment(lib, "Effect/Effects11d.lib")

/* DirectX Tex*/
#include "DirectXTex/DirectXTex.h"
#pragma comment(lib, "DirectXTex/DirectXTex.lib")

/* ImGui */
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#pragma comment(lib, "imgui/IMGUI.lib")

/* My Own */
#include "Definitions.h"
#include "Systems/D3D.h"
#include "Systems/Gui.h"
#include "Systems/Timer.h"
#include "Systems/Keyboard.h"
#include "Systems/Mouse.h"
#include "Viewer/Camera.h"
#include "Renders/Shader.h"
#include "Renders/VertexLayout.h"
#include "Renders/Texture.h"
#include "Utilites/Path.h"
#include "Utilites/String.h"

/* SDT_Math*/
#include "Core/Vector2D.h"
#include "Core/Vector.h"
#include "Core/Vector4.h"
#include "Core/Quaternion.h"
#include "Core/Matrix.h"
#include "Core/Plane.h"
#include "Core/Point.h"
#include "Core/Color.h"
#include "Core/Math.h"
