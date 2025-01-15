#pragma once

#include <Windows.h>
#include <assert.h>

#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
#include <chrono> // for timer, steady_clock
using namespace std;

#pragma region DirectX
	#include <d3dcommon.h>
	#include <d3d11.h>
	#include <d3dcompiler.h>
	#include <DirectXMath.h>

	#pragma comment(lib, "dxgi.lib")
	#pragma comment(lib, "d3d11.lib")
	#pragma comment(lib, "D3DCompiler.lib")
#pragma endregion


#pragma region Effect
	#include "Effect/d3dx11effect.h"
	#pragma comment(lib, "Effect/Effects11d.lib")
#pragma endregion

#pragma region DirectXTex
	#include "DirectXTex/DirectXTex.h"
	#pragma comment(lib, "DirectXTex/DirectXTex.lib")
#pragma endregion

#pragma region Json
	#include "JsonCpp/json.h"
	#pragma comment(lib, "JsonCpp/jsoncpp_static.lib")
#pragma endregion

#pragma region  ImGui 
	#include <imgui/imgui.h>
	#include <imgui/imgui_impl_dx11.h>
	#include <imgui/imgui_impl_win32.h>
#pragma comment(lib, "imgui/IMGUI.lib")

#pragma endregion

#pragma region  My Own 
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
	#include "Renders/Material.h"
	#include "Renders/Context.h"
	#include "Renders/Buffers.h"
	#include "Renders/ConstantDataBinder.h"
	#include "Renders/Transform.h"

	#include "Utilites/Path.h"
	#include "Utilites/String.h"
	#include "Utilites/BinaryFile.h"

	#include "Model/Model.h"
	#include "Model/ModelBone.h"
	#include "Model/ModelMesh.h"
	#include "Model/ModelAnimation.h"

	#include "Environment/Terrain.h"
#pragma endregion

#pragma region  SDT_Math
	#include "Core/Vector2D.h"
	#include "Core/Vector.h"
	#include "Core/Vector4.h"
	#include "Core/Quaternion.h"
	#include "Core/Matrix.h"
	#include "Core/Plane.h"
	#include "Core/Point.h"
	#include "Core/Color.h"
	#include "Core/Math.h"
#pragma endregion
