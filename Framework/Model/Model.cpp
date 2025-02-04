#include "framework.h"
#include "Model.h"
#include <string>
#include <unordered_map>

#include "SkeletalMesh.h"

Model::Model(const wstring & ModelFileName)
 // : RootBone(nullptr)
{
#ifdef DO_DEBUG
	WorldTransform = new Transform("Model WorldTransform of [" + String::ToString(ModelFileName) + "]");
#else
	WorldTransform = new Transform();
#endif
	const wstring FullFilePath = W_MODEL_PATH + ModelFileName + L".model";
	this->ModelName = String::ToString(ModelFileName);
	ReadFile(FullFilePath);
}

Model::Model( const wstring & ModelFileName, const Vector & Pos, const Quaternion & Rot, const Vector & Scale )
	// :RootBone(nullptr)
{
#ifdef DO_DEBUG
	WorldTransform = new Transform("Model WorldTransform of [" + String::ToString(ModelFileName) + "]");
#else
	WorldTransform = new Transform();
#endif
	WorldTransform->SetTRS(Pos,Rot,Scale);
	const wstring FullFilePath = W_MODEL_PATH + ModelFileName + L".model";
	this->ModelName = String::ToString(ModelFileName);
	ReadFile(FullFilePath);
}

Model::~Model()
{
	SAFE_RELEASE(ClipTexture);
	SAFE_RELEASE(ClipSRV);
	
	// for (const ModelBone * Bone : Bones)
	// 	SAFE_DELETE(Bone);
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
	if (Animations.empty() == false)
	{
		int clip = static_cast<int>(GetClipIndex());
		ImGui::SliderInt("Animation Clip #", &clip, 0, static_cast<int>(GetClipCount()) - 1);
		if (clip != static_cast<int>(GetClipIndex()))
			SetClipIndex(clip);
	}
	for (ModelMesh * M : Meshes)
	{
		M->SetWorldTransform(this->WorldTransform);
		const ModelAnimation * CurrentAnimation = nullptr;
		if (Animations.empty() == false)
			CurrentAnimation = Animations[GetClipIndex()];
		M->Tick(CurrentAnimation);
	}
}

void Model::Render()
{
	for (ModelMesh * const M : Meshes)
	{
		M->Render();
	}
}

void Model::SetPass( int InPass )
{
	Pass = InPass;
	for (ModelMesh * TargetMesh : Meshes)
	{
		TargetMesh->Pass = this->Pass;
	}
}

Color Model::JsonStringToColor( const Json::String & InJson )
{
	vector<Json::String> v;
	String::SplitString(&v, InJson, ",");

	return Color(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}

