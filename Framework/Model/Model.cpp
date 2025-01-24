#include "framework.h"
#include "Model.h"
#include <string>
#include <unordered_map>

Model::Model(const wstring & ModelFileName)
 : RootBone(nullptr)
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
	:RootBone(nullptr)
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
		mesh->SetWorldTransform(this->WorldTransform);
		mesh->Tick();
	}
}

void Model::Render()
{
	if (this->Animations.size() > 0)
	{
		static float time = 0;
		static int bPlay = 1;
		static float PlayRate = 1.f;
		int clip = (int)GetClipIndex();
		ImGui::SliderInt("Animation Clip #", &clip, 0, (int)GetClipCount() - 1);
		SetClipIndex(clip);
		ImGui::SliderInt("Play/Pause", &bPlay, 0, 1);
		// ImGui::SliderInt("Frame", &Frame, 0, Animations[0]->Duration);
		ImGui::SliderFloat("Speed", &PlayRate, 0.1f, 2.f);

		if (time > (1.f / (Animations[0]->TicksPerSecond * PlayRate)))
		{
			Frame += 1;
			Frame %= static_cast<int>(this->Animations[clip]->GetAnimationLength());
			time = 0;
		}
		if (bPlay == 1)
			time += Sdt::SystemTimer::Get()->GetDeltaTime();
	}
	for (ModelMesh * mesh : Meshes)
	{
		mesh->Render(Frame);
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
