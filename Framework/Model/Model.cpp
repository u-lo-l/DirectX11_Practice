#include "framework.h"
#include "Model.h"
#include <string>
#include <unordered_map>

#include "SkeletalMesh.h"

Model::Model(const wstring & ModelFileName)
{
#ifdef DO_DEBUG
	WorldTransform = new Transform("Model WorldTransform of [" + String::ToString(ModelFileName) + "]");
#else
	WorldTransform = new Transform();
#endif
	const wstring FullFilePath = W_MODEL_PATH + ModelFileName + L".model";
	this->ModelName = String::ToString(ModelFileName);
	ReadFile(FullFilePath);

	if (Animations.size() == 0)
		InstanceBuffer = new VertexBuffer(WorldTFMatrix, MaxModelInstanceCount, sizeof(Matrix), Shader::InstancingSlot, true);
}

Model::Model( const wstring & ModelFileName, const Vector & Pos, const Quaternion & Rot, const Vector & Scale )
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

	InstanceBuffer = new VertexBuffer(WorldTFMatrix, MaxModelInstanceCount, sizeof(Matrix), Shader::InstancingSlot);
}

Model::~Model()
{
	SAFE_DELETE(InstanceBuffer);
	SAFE_RELEASE(ClipTexture);
	SAFE_RELEASE(ClipSRV);
	
	for (const ModelMesh * Mesh : Meshes)
		SAFE_DELETE(Mesh);
	for (pair<string, Material *> KeyVal : MaterialsTable)
		SAFE_DELETE(KeyVal.second);
	for (const ModelAnimation * Animation : Animations)
		SAFE_DELETE(Animation);

	for (const Transform * Tf : WorldTransforms)
		SAFE_DELETE(Tf);
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
		M->Tick(GetCurrentAnimation());
	}
}

void Model::Render()
{
	if (InstanceBuffer != nullptr)
		InstanceBuffer->BindToGPU();
	for (ModelMesh * const M : Meshes)
	{
		M->Render((InstanceBuffer != nullptr));
	}
}

Color Model::JsonStringToColor( const Json::String & InJson )
{
	vector<Json::String> v;
	String::SplitString(&v, InJson, ",");

	return Color(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}

Transform * Model::AddTransforms()
{
	int index = WorldTransforms.size();
	Transform * NewTransform = new Transform(&WorldTFMatrix[index]);
	WorldTransforms.push_back(NewTransform);
	
	return NewTransform;
}

const Transform * Model::GetTransforms( UINT Index ) const
{
	if (Index >= WorldTransforms.size())
		return nullptr;
	return WorldTransforms[Index];
}

