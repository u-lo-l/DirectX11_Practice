#include "framework.h"
#include "Model.h"
#include <string>
#include <unordered_map>

#include "SkeletalMesh.h"

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
	if (this->Animations.empty() == true)
	{
		for (ModelMesh * M : Meshes)
		{
			M->SetWorldTransform(this->WorldTransform);
			M->Tick();
		}
		return;
	}
	
	int clip = static_cast<int>(GetClipIndex());
	ImGui::SliderInt("Animation Clip #", &clip, 0, static_cast<int>(GetClipCount()) - 1);
	if (clip != static_cast<int>(GetClipIndex()))
		SetClipIndex(clip);

	/*
	 * TODO : 이 부분 SkeletalMesh::Tick으로 옮기자
	 * 지금은 CalculateAnimationTime()때문에 힘듦
	 */
	for (ModelMesh * const M : Meshes)
	{
		SkeletalMesh * const Skm = dynamic_cast<SkeletalMesh * const>(M);
		if (Skm == nullptr)
			continue;
		SkeletalMesh::AnimationBlendingDesc & BlendingData = Skm->BlendingData; 
		
		CalculateAnimationTime(BlendingData.Current);
		
		if (BlendingData.Next.Clip > -1)
		{
			if (BlendingData.ElapsedBlendTime >= 1.0f) // Blending완료
			{
				BlendingData.Current = BlendingData.Next;
				BlendingData.Next.Clip = -1;
				BlendingData.ElapsedBlendTime = 0.0f;
			}
			else // Blending 중
			{
				CalculateAnimationTime(BlendingData.Next);
				const UINT ClipIndex = BlendingData.Next.Clip;
				const ModelAnimation * const TargetAnimation = this->Animations[ClipIndex];
				const float DeltaTime = Sdt::SystemTimer::Get()->GetDeltaTime();
				BlendingData.ElapsedBlendTime += DeltaTime / BlendingData.BlendingDuration;
			}
		}

		Skm->SetWorldTransform(this->WorldTransform);
		Skm->Tick();
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

/*
 * 현재 애니메이션의 TicksPerFrame이 필요함.
 * 현재 애니메이션의 Length가 필요함.
 * 현재 애니메이션의 PlayRate()가 필요함
 */
void Model::CalculateAnimationTime( SkeletalMesh::FrameDesc & FrameData ) const
{
	const UINT ClipIndex = GetClipIndex();
	const ModelAnimation * const TargetAnimation = this->Animations[ClipIndex];
	const float DeltaTime = Sdt::SystemTimer::Get()->GetDeltaTime() * TargetAnimation->TicksPerSecond;
	const UINT AnimationLength = static_cast<UINT>(TargetAnimation->GetAnimationLength());
	
	FrameData.CurrentTime += DeltaTime * TargetAnimation->GetPlayRate();
	FrameData.CurrentTime = fmod(FrameData.CurrentTime, static_cast<float>(AnimationLength));
	FrameData.CurrentFrame = static_cast<int>(FrameData.CurrentTime);
	FrameData.NextFrame = (UINT)((1 + FrameData.CurrentFrame) % AnimationLength);
}
