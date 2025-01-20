#include "Pch.h"

#include <fstream>
#include "ExportFile.h"
#include "Converter/Converter.h"

namespace Sdt
{
	const string ShaderName = "20_ModelVertex2.fx";
	const string ShaderForBoneIndicatorSphere= "21_ModelVertex3.fx";
	const string ShaderForAnimation = "22_Animation.fx";
	
	void ExportFile::Initialize()
	{
		// Mousey();
		Shannon();
		// Airplane();
		// Cube();
		// Manny();
		// YBot();
		// XYBot();
	}

	void ExportFile::MakeModelInfoFile( const wstring & InModelName )
	{
		Json::Value Root;

		Json::Value File;
		File["Material"] = String::ToString(InModelName);
		File["Mesh"] = String::ToString(InModelName);
		Root["File"] = File;

		Json::Value Transform;
		Transform["Position"] = "0,0,0";
		Transform["Rotation"] = "0,0,0";
		Transform["Scale"] = "0.01,0.01,0.01";
		Root["Transform"] = Transform;

		Root["Animations"].append(String::ToString(InModelName + L"/Idle"));

		Json::StyledWriter Writer;
		string Str = Writer.write(Root);

		ofstream ofs;
		wstring ModelFilePath = W_MODEL_PATH + InModelName + L".model";
		ofs.open(ModelFilePath);
		ofs << Str;
		ofs.close();
	}

	void ExportFile::Airplane()
	{
		Converter * converter = nullptr;
		converter = new Converter();
		converter->ReadAiSceneFromFile(L"Airplane/Airplane.fbx");
		converter->ExportMaterial(L"Airplane/Airplane", ShaderForAnimation, true);
		converter->ExportMesh(L"Airplane/Airplane");
		
		MakeModelInfoFile(L"Airplane");
		SAFE_DELETE(converter);
	}

	void ExportFile::Cube()
	{
		Converter* converter = new Converter();
		converter->ReadAiSceneFromFile(L"Cube/Cube.fbx");
		converter->ExportMaterial(L"Cube/Cube", ShaderForAnimation, true);
		converter->ExportMesh(L"Cube/Cube");
		
		MakeModelInfoFile(L"Cube");
		SAFE_DELETE(converter);
	}

	void ExportFile::Sphere()
	{
		Converter* converter = new Converter();
		converter->ReadAiSceneFromFile(L"Sphere/Sphere.fbx");
		converter->ExportMaterial(L"Sphere/Sphere", ShaderForBoneIndicatorSphere, true);
		converter->ExportMesh(L"Sphere/Sphere");
		MakeModelInfoFile(L"Sphere");
		SAFE_DELETE(converter);
	}

	void ExportFile::Shannon()
	{
		Converter * converter = new Converter();
		converter->ReadAiSceneFromFile(L"Shannon/Shannon.fbx");
		converter->ExportMaterial(L"Shannon/Shannon", ShaderForAnimation, true);
		converter->ExportMesh(L"Shannon/Shannon");

		converter->ReadAiSceneFromFile(L"Shannon/Idle.fbx");
		converter->ExportAnimation("Shannon/Idle", 0);
		SAFE_DELETE(converter);
		
		MakeModelInfoFile(L"Shannon");
	}
	
	void ExportFile::Mousey()
	{
		Converter* converter = new Converter();
		converter->ReadAiSceneFromFile(L"Mousey/Mousey.fbx");
		converter->ExportMaterial(L"Mousey/Mousey", ShaderForAnimation, true);
		converter->ExportMesh(L"Mousey/Mousey");
		MakeModelInfoFile(L"Mousey");
		SAFE_DELETE(converter);
	}

	void ExportFile::Manny()
	{
		Converter* converter = new Converter();
		converter->ReadAiSceneFromFile(L"Manny/Manny.fbx");
		converter->ExportMaterial(L"Manny/Manny", ShaderForAnimation, true);
		converter->ExportMesh(L"Manny/Manny");
		MakeModelInfoFile(L"Manny");
		SAFE_DELETE(converter);
	}

	void ExportFile::YBot()
	{
		Converter* converter = new Converter();
		converter->ReadAiSceneFromFile(L"YBot/YBot.fbx");
		converter->ExportMaterial(L"YBot/YBot", ShaderForAnimation, true);
		converter->ExportMesh(L"YBot/YBot");
		MakeModelInfoFile(L"YBot");
		SAFE_DELETE(converter);
	}

	void ExportFile::XYBot()
	{
		Converter* converter = new Converter();
		converter->ReadAiSceneFromFile(L"XYBot/XYBot.fbx");
		converter->ExportMaterial(L"XYBot/XYBot", ShaderForAnimation, true);
		converter->ExportMesh(L"XYBot/XYBot");
		MakeModelInfoFile(L"XYBot");
		SAFE_DELETE(converter);
	}
}
