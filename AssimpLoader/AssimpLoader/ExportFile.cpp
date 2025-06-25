#include "Pch.h"
#include <fstream>
#include "ExportFile.h"
#include "Converter/MeshConverter.h"
#include "Converter/AnimationConverter.h"

namespace sdt
{
	// const string ShaderForAnim = "23_AnimationTwinning.fx";
	// const string ShaderForAnim = "38_Instancing_Model_Animation.fx";
	// const string ShaderForNonAnim = "38_Instancing_Model.fx";
	// const string ShaderForAnim = "38_Instancing_Model_Animation.hlsl";
	// const string ShaderForNonAnim = "38_Instancing_Model.hlsl";
	// const string ShaderForAnim = "Lighting/43_Instancing_Model_Animation_Lighting.hlsl";
	// const string ShaderForNonAnim = "Lighting/43_Instancing_Model_Lighting.hlsl";

	ExportFile::ExportFile()
	{
		bTickable = false;
	}

	ExportFile::~ExportFile()
	{
	}

	void ExportFile::Initialize()
	{
		const string ShaderForModel = "SkeletalMesh";
		ASSERT(ShaderManager::Get()->IsValidRenderingShaderName(ShaderForModel) == true, "ShaderName Not Valid");
		ExportMeshes({L"Adam/Adam.fbx", L"Cylinder/Cylinder.fbx", L"Cube/Cube.fbx"});
		ExportAnimations({
			L"Adam/RollFront.fbx",
			L"Adam/Locomotion/Stop.fbx",
			L"Adam/Locomotion/Walk_F.fbx",
			L"Adam/Locomotion/Walk_B.fbx",
			L"Adam/Locomotion/Walk_R.fbx",
			L"Adam/Locomotion/Walk_L.fbx",
			L"Adam/Locomotion/Run_F.fbx",
			L"Adam/Locomotion/Run_B.fbx",
			L"Adam/Locomotion/Run_R.fbx",
			L"Adam/Locomotion/Run_L.fbx",
		});
	}

	void ExportFile::ExportMeshes(const vector<wstring> & FBXFileNames)
	{
		MeshConverter * meshConverter = new MeshConverter();
		for (const wstring & fileName : FBXFileNames)
			meshConverter->ReadAiScene(fileName);
		SAFE_DELETE(meshConverter);
	}

	void ExportFile::ExportAnimations(const vector<wstring>& FBXFileNames)
	{
		AnimationConverter * AnimConverter = new AnimationConverter();
		for (const wstring & fileName : FBXFileNames)
			AnimConverter->ReadAiScene(fileName);
		SAFE_DELETE(AnimConverter);
	}
}
