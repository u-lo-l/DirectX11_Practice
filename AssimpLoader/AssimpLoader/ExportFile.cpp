#include "Pch.h"

#include <fstream>
#include "ExportFile.h"
#include "Converter.h"

namespace Sdt
{

	void ExportFile::Initialize()
	{
		this->Mousey();
		this->Shannon();
		this->Airplane();
		this->Cube();
	}

	void ExportFile::MakeModelInfoFile( const wstring & InModelName )
	{
		Json::Value Root;

		Json::Value File;
		File["Material"] = String::ToString(InModelName);
		File["Mesh"] = String::ToString(InModelName);

		Json::Value Transform;
		Transform["Position"] = "0,0,0";
		Transform["Rotation"] = "0,0,0";
		Transform["Scale"] = "1,1,1";

		Root["File"] = File;
		Root["Transform"] = Transform;

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
		Converter* converter = new Converter();
		converter->ReadFile(L"Airplane/Airplane.fbx");
		converter->ExportMaterial(L"Airplane/Airplane", true);
		converter->ExportMesh(L"Airplane/Airplane");
		MakeModelInfoFile(L"Airplane");
		SAFE_DELETE(converter);
	}

	void ExportFile::Cube()
	{
		Converter* converter = new Converter();
		converter->ReadFile(L"Cube/Cube.fbx");
		converter->ExportMaterial(L"Cube/Cube", true);
		converter->ExportMesh(L"Cube/Cube");
		MakeModelInfoFile(L"Cube");
		SAFE_DELETE(converter);
	}

	void ExportFile::Shannon()
	{
		Converter* converter = new Converter();
		converter->ReadFile(L"Shannon/Shannon.fbx");
		converter->ExportMaterial(L"Shannon/Shannon", true);
		converter->ExportMesh(L"Shannon/Shannon");
		MakeModelInfoFile(L"Shannon");
		SAFE_DELETE(converter);
	}
	
	void ExportFile::Mousey()
	{
		Converter* converter = new Converter();
		converter->ReadFile(L"Mousey/Mousey.fbx");
		converter->ExportMaterial(L"Mousey/Mousey", true);
		converter->ExportMesh(L"Mousey/Mousey");
		MakeModelInfoFile(L"Mousey");
		SAFE_DELETE(converter);
	}
}
