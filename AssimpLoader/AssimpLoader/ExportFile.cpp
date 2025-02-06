#include "Pch.h"
#include <fstream>
#include "ExportFile.h"
#include "Converter/Converter.h"

namespace Sdt
{
	// const string ShaderForAnim = "23_AnimationTwinning.fx";
	const string ShaderForAnim = "38_Instancing_Model_Animation.fx";
	const string ShaderForNonAnim = "38_Instancing_Model.fx";
	
	void ExportFile::Initialize()
	{
		// MakeModel(L"Adam", {});
		// MakeModel(L"Adam", {L"Idle", L"Dance01", L"Dance02", L"Dance03"}, 1.f);
		// MakeModel(L"Airplane", {}, 0.1f);
		// MakeModel(L"Cube", {}, 1.f);
		// MakeModel(L"Cylinder", {}, 1.f);
		// MakeModel(L"Sphere", {}, 1.f);
		// MakeModel(L"Cone", {}, 1.f);
		// MakeModel(L"Plane", {}, 10.f);
		// MakeModel(L"Mousey", {}, 1.f);
		// MakeModel(L"XYBot", {}, 1.f);
		// MakeModel(L"Shannon", {}, 1.f);
		MakeModel(L"Kachujin", {}, 1.f);
	}

	void ExportFile::MakeModel(const wstring & InModelName, const vector<wstring> & InAnimationNames, float InScale)
	{
		Converter * converter = new Converter();
		converter->ReadAiSceneFromFile(InModelName + L"/" + InModelName + L".fbx");
		if (InAnimationNames.empty())
			converter->ExportMaterial(InModelName + L"/" + InModelName, ShaderForNonAnim, true);
		else
			converter->ExportMaterial(InModelName + L"/" + InModelName, ShaderForAnim, true);
		converter->ExportMesh(InModelName + L"/" + InModelName);
		
		MakeModelInfoFile(InModelName, InAnimationNames, InScale);

		for (wstring AnimationName : InAnimationNames)
		{
			converter->ReadAiSceneFromFile(InModelName + L"/" + AnimationName + L".fbx");
			converter->ExportAnimation(String::ToString(InModelName) + "/" + String::ToString(AnimationName), 0);
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
