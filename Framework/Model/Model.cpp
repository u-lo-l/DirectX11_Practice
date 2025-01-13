// ReSharper disable All
#include "framework.h"
#include "Model.h"
#include <string>
#include <fstream>

Model::Model(const wstring & ModelFileName)
 : RootBone(nullptr)
{

	WorldTransform = new Transform();
	wstring FullFilePath = W_MODEL_PATH + ModelFileName + L".model";
	ReadFile(FullFilePath);
}

Model::~Model()
{
	for (const ModelBone * Bone : Bones)
		SAFE_DELETE(Bone);
	for (const ModelMesh * Mesh : Meshes)
		SAFE_DELETE(Mesh);

	for (pair<string, Material *> KeyVal : MaterialsTable)
		SAFE_DELETE(KeyVal.second);
	
	SAFE_DELETE(WorldTransform);
}

void Model::Tick() const
{
	for (ModelMesh * mesh : Meshes)
	{
		mesh->SetWorldTransform(this->WorldTransform);
		mesh->Tick();
	}
}

void Model::Render() const
{
	for (ModelMesh * mesh : Meshes)
		mesh->Render();
}

void Model::ReadFile( const wstring & InFileFullPath )
{
	ifstream ifs;
	ifs.open(InFileFullPath);

	Json::Value Root;
	ifs >> Root;

	Json::Value::Members Members = Root.getMemberNames();
	printf("Member Count : %d\n", Members.size());
	
	Json::Value material = Root["File"]["Material"];
	Json::Value Mesh = Root["File"]["Mesh"];
	Json::Value Position = Root["Transform"]["Position"];
	Json::Value Rotation = Root["Transform"]["Rotation"];
	Json::Value Scale = Root["Transform"]["Scale"];
	ifs.close();

	wstring MaterialName = String::ToWString(material.asString());
	wstring MeshName = String::ToWString(Mesh.asString());
	
	ReadMaterial(MaterialName);
	ReadMesh(MeshName + L"/" + MeshName);

	if (WorldTransform == nullptr)
		WorldTransform = new Transform();

	vector<string> pString;
	String::SplitString(&pString, Position.asString(), ",");
	WorldTransform->SetPosition({stof(pString[0]), stof(pString[1]), stof(pString[2])});

	String::SplitString(&pString, Rotation.asString(), ",");
	WorldTransform->SetPosition({stof(pString[0]), stof(pString[1]), stof(pString[2])});

	String::SplitString(&pString, Scale.asString(), ",");
	WorldTransform->SetPosition({stof(pString[0]), stof(pString[1]), stof(pString[2])});
}


void Model::ReadMaterial( const wstring & InFileName)
{
	const wstring FullPath = W_MATERIAL_PATH + InFileName + L".material";
	
	ifstream Stream;
	Stream.open(FullPath);

	Json::Value Root;
	Stream >> Root;
	
	Json::Value::Members Members = Root.getMemberNames();
	for (const Json::String & Name : Members)
	{
		Material * MatData = new Material();
		Json::Value Value = Root[Name];

		if (Value["ShaderName"].asString().size() > 0)
			MatData->SetShader(String::ToWString(Value["ShaderName"].asString()));

		MatData->SetAmbient(JsonStringToColor(Value["Ambient"].asString()));
		MatData->SetDiffuse(JsonStringToColor(Value["Diffuse"].asString()));
		MatData->SetSpecular(JsonStringToColor(Value["Specular"].asString()));
		MatData->SetEmissive(JsonStringToColor(Value["Emissive"].asString()));

		UINT count = Value["DiffuseMap"].size();
		for (UINT i = 0; i < count; i++)
		{
			if (Value["DiffuseMap"][i].asString().size() > 0)
				MatData->SetDiffuseMap(String::ToWString(Value["DiffuseMap"][i].asString()));
		}

		count = Value["SpecularMap"].size();
		for (UINT i = 0; i < count; i++)
		{
			if (Value["SpecularMap"][i].asString().size() > 0)
				MatData->SetSpecularMap(String::ToWString(Value["SpecularMap"][i].asString()));
		}

		count = Value["NormalMap"].size();
		for (UINT i = 0; i < count; i++)
		{
			if (Value["NormalMap"][i].asString().size() > 0)
				MatData->SetNormalMap(String::ToWString(Value["NormalMap"][i].asString()));
		}

		MaterialsTable[Name] = MatData;
	}

	Stream.close();
}

void Model::ReadMesh( const wstring & InFileName)
{
	const wstring FullPath = W_MODEL_PATH + InFileName + L".mesh";

	BinaryReader * BinReader = new BinaryReader();
	BinReader->Open(FullPath);

	ModelBone::ReadFile(BinReader, this->Bones);
	ModelMesh::ReadFile(BinReader, this->Meshes, this->MaterialsTable);

	BinReader->Close();
	SAFE_DELETE(BinReader);
	
	int count = 0;
	for (ModelBone * Bone : this->Bones)
	{
		this->BoneTransforms[count++] = Bone->Transform;
		for (UINT number : Bone->MeshIndices)
		{
			Meshes[number]->BoneIndex = Bone->Index;
			Meshes[number]->Bone = Bone;
			Meshes[number]->Transforms = BoneTransforms;
		}
	}

}

Color Model::JsonStringToColor( const Json::String & InJson )
{
	vector<Json::String> v;
	String::SplitString(&v, InJson, ",");

	return {stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3])};
}
