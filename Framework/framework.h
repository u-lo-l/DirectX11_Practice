#pragma once

// #ifndef DISPLAY_IMGUI_DEBUG_INFO
// # define DISPLAY_IMGUI_DEBUG_INFO
// #endif
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
	#include <DirectXPackedVector.h>

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
	#include "Math/Box.h"
	#include "Math/Frustum.h"
	#include "Math/Noise.h"
	#include "Math/Math.h"

	#include "BasicShapes/BasicShapes2D/Circle2D.h"
	#include "BasicShapes/BasicShapes2D/LineSegment2D.h"
	#include "BasicShapes/BasicShapes2D/Triangle2D.h"
#pragma endregion

#pragma region  My Own
	#include "Structures/Structures.h"
	#include "Structures/D3DResource.h"
	#include "Structures/KeyFrameAnimation.h"
	#include "Structures/Weather.h"
	#include "Structures/Ligthting.h"
	#include "Structures/Shader.h"

	#include "Utilites/Hash.h"
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

	#include "Renders/Buffer/Buffers.h"

	#include "Renders/Texture.h"
	#include "Renders/TextureArray.h"
	#include "Renders/RWTexture2D.h"
	#include "Renders/RWTexture2DArray.h"
	#include "Renders/Context.h"
	#include "Renders/ConstantDataBinder.h"
	#include "Renders/Transform.h"
	#include "Renders/Projector/Projection.h"
	#include "Renders/ViewPort/ViewPort.h"
	#include "Renders/Projector/Orthographic.h"
	#include "Renders/Projector/Perspective.h"

	#include "Renders/Shader/VertexLayout.h"
	#include "Renders/Shader/ShaderBase.h"
	#include "Renders/Shader/RenderingShader.h"
	#include "Renders/Shader/ComputeShader.h"
	#include "Renders/Shader/ShaderManager.h"
	#include "Renders/Renderable/ARenderable.h"
	#include "Renders/Renderable/RenderManager.h"

	#include "Material/Material.h"
	#include "Material/MeshMaterial.h"
	#include "Material/TerrainMaterial.h"
	#include "Material/LineMaterial.h"

	#include "Viewer/Camera.h"
	#include "Viewer/DepthStencil.h"
	#include "Viewer/RenderTarget.h"

	#include "Model/Mesh/CBone.h"
	#include "Model/Mesh/CMesh.h"
	#include "Model/Mesh/CSkeletal.h"
	#include "Model/Mesh/SkeletalMeshSubset.h"
	#include "Model/Mesh/StaticMeshSubset.h"
	#include "Model/Mesh/CSkeletalMesh.h"
	#include "Model/Mesh/CStaticMesh.h"

	#include "Model/Animation/AnimationClip.h"
	#include "Model/Animation/AnimationBlendSpace1D.h"
	#include "Model/Animation/AnimationBlendSpaceHelper.h"
	#include "Model/Animation/AnimationBlendSpace2D.h"
	#include "Model/Animation/AnimationController.h"

	#include "Environment/LandScape/SceneryCell.h"
	#include "Environment/LandScape/LandScape.h"
	// #include "Environment/Ocean/Ocean.h"
	#include "Environment/SkySphere.h"
	// #include "Environment/Foliage.h"

	#include "ParticleSystem/ParticleSystem.h"

	#include "Ligthing/LightingManager.h"

	#include "Objects/Billboard/Billboard.h"
	#include "Objects/CrossQuad/CrossQuad.h"
	#include "Objects/PostEffect/PostEffect.h"
	#include "Objects/Projector/Projector.h"
	#include "Objects/Shadow/Shadow.h"
#pragma endregion

string GetD3D11ReturnMessage(HRESULT Hr);
