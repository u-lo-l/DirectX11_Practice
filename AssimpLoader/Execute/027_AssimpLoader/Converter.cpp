#include "Pch.h"
#include "Converter.h"

#include <fstream>

namespace Sdt
{
	Converter::Converter()
	 : Loader(nullptr), Scene(nullptr)
	{
		Loader = new Assimp::Importer();
	}

	Converter::~Converter()
	{
		Loader->FreeScene();
		SAFE_DELETE(Loader);
	}

	void Converter::ReadFile( const wstring & InFileName )
	{
		string FileName = String::ToString(InFileName);
		FileName = "../../_Assets/" + FileName;

		Scene = Loader->ReadFile(
			FileName.c_str(),
			aiProcess_ConvertToLeftHanded
			| aiProcess_Triangulate
			| aiProcess_GenUVCoords
			| aiProcess_GenNormals
			| aiProcess_CalcTangentSpace
			| aiProcess_GenBoundingBoxes
		);
		ASSERT(Scene != nullptr, Loader->GetErrorString());
	}

	void Converter::ExportMaterial( const wstring & InSaveFileName, bool InbOverwrite )
	{
		wstring SaveFileName = L"../../_Materials/" + InSaveFileName + L".material";
		ReadMaterial();
		WriteMaterial(SaveFileName, InbOverwrite);
	}

	void Converter::ReadMaterial()
	{
		printf("mNumMaterials = %d\n", Scene->mNumMaterials);
		for (UINT i = 0; i < Scene->mNumMaterials; i++)
		{
			const aiMaterial* Material = Scene->mMaterials[i];
			MaterialData * Data = nullptr;
			Data = new MaterialData();

			Data->Name = Material->GetName().C_Str();
			Data->ShaderName = "";
		
		
			aiColor4D color;
		
			Material->Get(AI_MATKEY_COLOR_AMBIENT, color);
			Data->Ambient = Color(color.r, color.g, color.b, color.a);
		
			Material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			Data->Diffuse = Color(color.r, color.g, color.b, color.a);
		
			Material->Get(AI_MATKEY_COLOR_SPECULAR, color);
			Data->Specular = Color(color.r, color.g, color.b, color.a);
		
			float shininess = 0.0f;
			Material->Get(AI_MATKEY_SHININESS, shininess);
			Data->Shininess = shininess;
		
			Material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
			Data->Emissive = Color(color.r, color.g, color.b, color.a);
		
			Materials.push_back(Data);
		}
	}

	void Converter::WriteMaterial( const wstring & InSaveFileName, bool InbOverwrite ) const
	{
		if (InbOverwrite == false)
		{
			if (Path::IsFileExist(InSaveFileName) == true)
				return;
		}

		string FolderName = String::ToString(Path::GetDirectoryName(InSaveFileName));
		string FileName = String::ToString(Path::GetFileName(InSaveFileName));

		Path::CreateFolders(FolderName);

		Json::Value Root;

		for (const MaterialData* Data : Materials)
		{
			Json::Value ShaderName;
			ShaderName["ShaderName"] = Data->ShaderName;
		
			Json::Value color;
			color["Ambient"].append(Data->Ambient.ToString().c_str());
			color["Diffuse"].append(Data->Diffuse.ToString().c_str());
			color["Specular"].append(Data->Specular.ToString().c_str());
		
			Root[Data->Name.c_str()].append(ShaderName);
			Root[Data->Name.c_str()].append(color);
		}
		
		Json::StyledWriter JsonWriter;
		string TempStr = JsonWriter.write(Root);
		
		ofstream Ofstream;
		Ofstream.open(InSaveFileName);
		Ofstream << TempStr;
		Ofstream.close();
	}
}
