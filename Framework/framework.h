#pragma once

// #define DO_DEBUG

#include <Windows.h>
#include <assert.h>

#include <string>
#include <vector>
#include <set>
#include <map>
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

#pragma region  SDT_Math
	#include "Math/Vector2D.h"
	#include "Math/Vector.h"
	#include "Math/Vector4.h"
	#include "Math/Quaternion.h"
	#include "Math/Matrix.h"
	#include "Math/Plane.h"
	#include "Math/Point.h"
	#include "Math/Color.h"
	#include "Math/Math.h"
#pragma endregion


#pragma region  My Own 
	#include "Utilites/Definitions.h"
	#include "Utilites/ShaderSlots.h"
	#include "Utilites/Helper.h"
	#include "Utilites/Path.h"
	#include "Utilites/String.h"
	#include "Utilites/BinaryFile.h"
	
	#include "Systems/D3D.h"
	#include "Systems/Gui.h"
	#include "Systems/SystemTimer.h"
	#include "Systems/Keyboard.h"
	#include "Systems/Mouse.h"

	#include "Renders/Shader/VertexLayout.h"
	#include "Renders/Shader/HlslShader.h"
	#include "Renders/Shader/HlslComputeShader.h"
	#include "Renders/Texture.h"
	#include "Renders/TextureArray.h"
	#include "Renders/Material.h"
	#include "Renders/Context.h"
	#include "Renders/Buffer/Buffers.h"
	#include "Renders/ConstantDataBinder.h"
	#include "Renders/Transform.h"
	#include "Renders/Projector/Projection.h"
	#include "Renders/ViewPort/ViewPort.h"
	#include "Renders/Projector/Orthographic.h"
	#include "Renders/Projector/Perspective.h"

	#include "Viewer/Camera.h"
	#include "Viewer/DepthStencil.h"
	#include "Viewer/RenderTarget.h"

	#include "Model/Model.h"
	#include "Model/ModelBone.h"
	#include "Model/ModelMesh.h"
	#include "Model/ModelAnimation.h"

	#include "Environment/Terrain.h"
	#include "Environment/Terrain2.h"
	#include "Environment/SkySphere.h"

	#include "ParticleSystem/ParticleSystem.h"

	#include "Ligthing/LightingManager.h"

	#include "Objects/Billboard/Billboard.h"
	#include "Objects/CrossQuad/CrossQuad.h"
	#include "Objects/PostEffect/PostEffect.h"
	#include "Objects/Projector/Projector.h"
	#include "Objects//Shadow/Shadow.h"

#pragma endregion

string GetD3D11ReturnMessage(HRESULT Hr);
