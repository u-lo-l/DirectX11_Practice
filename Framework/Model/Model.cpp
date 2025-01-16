#include "framework.h"
#include "Model.h"
#include <string>
#include <unordered_map>

Model::Model(const wstring & ModelFileName)
 : RootBone(nullptr)
{
	WorldTransform = new Transform();
	wstring FullFilePath = W_MODEL_PATH + ModelFileName + L".model";
	this->ModelName = String::ToString(ModelFileName);
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
	for (const ModelAnimation * Animation : Animations)
		SAFE_DELETE(Animation);
	
	SAFE_DELETE(WorldTransform);
}

void Model::Tick()
{
	for (ModelMesh * mesh : Meshes)
	{
		if (bTransformChanged == true)
		{
			mesh->SetWorldTransform(this->WorldTransform);
			bTransformChanged = false;
		}
		mesh->Tick();
	}
}

void Model::Render() const
{
	for (ModelMesh * mesh : Meshes)
	{
		mesh->Pass = this->Pass;
		mesh->Render();
	}
}

void Model::SetPass( int InPass )
{
	Pass = InPass;
}

Color Model::JsonStringToColor( const Json::String & InJson )
{
	vector<Json::String> v;
	String::SplitString(&v, InJson, ",");

	return Color(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}
