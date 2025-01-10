// ReSharper disable All
#include "framework.h"
#include "Model.h"
#include <string>
#include <fstream>

Model::Model()
 : RootBone(nullptr)
{
	
}

Model::~Model()
{
	for (const ModelBone * Bone : Bones)
		SAFE_DELETE(Bone);
	for (const ModelMesh * Mesh : Meshes)
		SAFE_DELETE(Mesh);
}

void Model::Tick() const
{
	for (ModelMesh * mesh : Meshes)
		mesh->Tick();
}

void Model::Render() const
{
	for (ModelMesh * mesh : Meshes)
		mesh->Render();
}


void Model::ReadMaterial( const wstring & InFileName)
{
	const wstring FullPath = W_MATERIAL_PATH + InFileName + L".material";
	
#pragma region ifstream
	ifstream Stream;
	Stream.open(FullPath);

	Json::Value Root;
	Stream >> Root;
	
	Json::Value::Members Members = Root.getMemberNames();
	for (const Json::String & Name : Members)
	{
		Material * MatData = new Material();

		Json::Value::ArrayIndex Index = Root[Name].size();
		for (UINT i = 0; i < Index; i++)
		{
			Json::Value Value = Root[Name][i];
			Json::Value::Members ValueMembers = Value.getMemberNames();
			for (const Json::String & ValueName : ValueMembers)
			{
				if (ValueName.compare("ShaderName") == 0)
				{
					if (Value[ValueName].asString().size() > 0)
					{
						const wstring ShaderName = String::ToWString(Value[ValueName].asString());
						MatData->SetShader(ShaderName);
					}
				}
				else if (ValueName == "Ambient")
				{
					Json::String str = Value[ValueName].asString();
					Color AmbientColor = JsonStringToColor(str);
					MatData->SetAmbient(AmbientColor);
				}
				else if (ValueName == "Diffuse")
				{
					Json::String str = Value[ValueName].asString();
					Color DiffuseColor = JsonStringToColor(str);
					MatData->SetDiffuse(DiffuseColor);
				}
				else if (ValueName.compare("Specular") == 0)
				{
					MatData->SetSpecular(JsonStringToColor(Value[ValueName].asString()));
				}
				else if (ValueName.compare("Emissive") == 0)
				{
					MatData->SetEmissive(JsonStringToColor(Value[ValueName].asString()));
				}
				else if (ValueName.compare("DiffuseMap") == 0 && Value[ValueName].size() > 0 && Value[ValueName][0].asString().size() > 0)
				{
					MatData->SetDiffuseMap(String::ToWString(Value[ValueName][0].asString()));
				}
				else if (ValueName.compare("SpecularMap") == 0 && Value[ValueName].size() > 0 &&Value[ValueName][0].asString().size() > 0)
				{
					MatData->SetSpecularMap(String::ToWString(Value[ValueName][0].asString()));
				}
				else if (ValueName.compare("NormalMap") == 0 && Value[ValueName].size() > 0 && Value[ValueName][0].asString().size() > 0)
				{
					MatData->SetNormalMap(String::ToWString(Value[ValueName][0].asString()));
				}
			}
		}

		MaterialsTable[Name] = MatData;
	}

	Stream.close();
#pragma endregion
}

void Model::ReadMesh( const wstring & InFileName)
{
	const wstring FullPath = W_MODEL_PATH + InFileName + L".mesh";

	BinaryReader * BinReader = new BinaryReader();
	BinReader->Open(FullPath);

	ModelBone::ReadFile(BinReader, this->Bones);
	ModelMesh::ReadFile(BinReader, this->Meshes, this->MaterialsTable);

	for (ModelMesh* mesh : Meshes)
		mesh->BindData();
	
	BinReader->Close();
	SAFE_DELETE(BinReader);
}

Color Model::JsonStringToColor( const Json::String & InJson )
{
	vector<Json::String> v;
	String::SplitString(&v, InJson, ",");

	return {stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3])};
}
