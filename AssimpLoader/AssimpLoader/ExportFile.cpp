#include "Pch.h"
#include <fstream>
#include "ExportFile.h"
#include "Converter/Converter.h"
#include "Converter2/MeshConverter.h"
#include "Converter2/AnimationConverter.h"

namespace sdt
{
	// const string ShaderForAnim = "23_AnimationTwinning.fx";
	// const string ShaderForAnim = "38_Instancing_Model_Animation.fx";
	// const string ShaderForNonAnim = "38_Instancing_Model.fx";
	// const string ShaderForAnim = "38_Instancing_Model_Animation.hlsl";
	// const string ShaderForNonAnim = "38_Instancing_Model.hlsl";
	// const string ShaderForAnim = "Lighting/43_Instancing_Model_Animation_Lighting.hlsl";
	// const string ShaderForNonAnim = "Lighting/43_Instancing_Model_Lighting.hlsl";

	const string ShaderForModel = "Model/Model.hlsl";

	ExportFile::ExportFile()
	{
		bTickable = false;
	}

	void ExportFile::Initialize()
	{
		ExportMeshes({L"Adam/Adam.fbx", L"Kachujin/Kachujin.fbx"});
		ExportAnimations({
			L"Adam/Stop.fbx",
			L"Adam/Walk_F.fbx",
		});
	}

	void ExportFile::ExportMeshes(const vector<wstring>& FBXFileNames)
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

	void ExportFile::MakeModel(const wstring & InModelName, const vector<wstring> & InAnimationNames, float InScale)
	{
		Converter * converter = new Converter();
		converter->ReadAiSceneFromFile(InModelName + L"/" + InModelName + L".fbx");
		if (InAnimationNames.empty())
			converter->ExportMaterial(InModelName + L"/" + InModelName, ShaderForModel, true);
		else
			converter->ExportMaterial(InModelName + L"/" + InModelName, ShaderForModel, true);
		converter->ExportMesh(InModelName + L"/" + InModelName);
		
		MakeModelInfoFile(InModelName, InAnimationNames, InScale);

		for (wstring AnimationName : InAnimationNames)
		{
			converter->ReadAiSceneFromFile(InModelName + L"/" + AnimationName + L".fbx");
			converter->ExportAnimation(
				String::ToString(InModelName) + "/" + String::ToString(AnimationName),
				0
			);
		}
		SAFE_DELETE(converter);
	}
	
	void ExportFile::MakeModelInfoFile( const wstring & InModelName, const vector<wstring> & InAnimationNames, float InScale)
	{
		Json::Value Root;

		Json::Value File;
		File["Material"] = String::ToString(InModelName);
		File["Mesh"] = String::ToString(InModelName);
		Root["File"] = File;

		Json::Value Transform;
		Transform["Position"] = "0,0,0";
		Transform["Rotation"] = "0,0,0";
		string ScaleFactor = std::to_string(InScale);
		Transform["Scale"] = ScaleFactor+","+ScaleFactor+","+ScaleFactor;
		Root["Transform"] = Transform;
		for (const wstring & AnimationName : InAnimationNames)
			Root["Animations"].append(String::ToString(InModelName + L"/" + AnimationName));

		Json::StyledWriter Writer;
		string Str = Writer.write(Root);

		ofstream ofs;
		wstring ModelFilePath = W_MODEL_PATH + InModelName + L".model";
		ofs.open(ModelFilePath);
		ofs << Str;
		ofs.close();
	}
}
