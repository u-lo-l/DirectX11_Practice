#include "framework.h"
#include "Model.h"
#include <string>
#include <unordered_map>

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

	for (ModelMesh * M : Meshes)
		M->Pass = 0;
	
	InstanceBuffer = new VertexBuffer(
							WorldTFMatrix,
							MaxModelInstanceCount,
							sizeof(Matrix),
							Shader::InstancingSlot,
							true
						);
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

	for (ModelMesh * M : Meshes)
		M->Pass = (Animations.empty() == true) ? 0 : 1;
	
	InstanceBuffer = new VertexBuffer(
							WorldTFMatrix,
							MaxModelInstanceCount,
							sizeof(Matrix),
							Shader::InstancingSlot,
							true
						);
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
	for (ModelMesh * M : Meshes)
	{
		M->Tick(WorldTransforms.size(), Animations);
	}
}

void Model::Render()
{
	// Model에서 InstanceBuffer를 Bind해주면 Mesh들에서 갖다 쓴다.
	// 여기서 Bind해주는 정보는 Model의 World기준 Transform Matrix이다.
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
	const int Index = WorldTransforms.size();
	Transform * NewTransform = new Transform(&WorldTFMatrix[Index]);
	WorldTransforms.push_back(NewTransform);

	if (Animations.empty() == false)
	{
		SetClipIndex(Index, 0);
	}
	
	return NewTransform;
}

const Transform * Model::GetTransforms( UINT Index ) const
{
	if (Index >= WorldTransforms.size())
		return nullptr;
	return WorldTransforms[Index];
}

void Model::SetClipIndex(UINT InInstanceID, UINT InClipIndex )
{
	ASSERT(InClipIndex < Animations.size(), "Animation Index Not Valid");
	for (ModelMesh * const TargetMesh : Meshes)
	{
		if (TargetMesh->BlendingDatas[InInstanceID].Current.Clip < 0) // 처음 초기화 될 때.
		{
			TargetMesh->BlendingDatas[InInstanceID].Current.Clip = InClipIndex;
			TargetMesh->BlendingDatas[InInstanceID].Current.CurrentTime = 0;
			TargetMesh->BlendingDatas[InInstanceID].Current.CurrentFrame = 0;
			TargetMesh->BlendingDatas[InInstanceID].Current.NextFrame = 0;

			TargetMesh->BlendingDatas[InInstanceID].Next.Clip = -1;
		}
		else if (InClipIndex != TargetMesh->BlendingDatas[InInstanceID].Current.Clip)// Current에서 Next로 애니메이션이 바뀜.
		{
			TargetMesh->BlendingDatas[InInstanceID].BlendingDuration = 0.1f;
			TargetMesh->BlendingDatas[InInstanceID].ElapsedBlendTime = 0.0f;
			
			TargetMesh->BlendingDatas[InInstanceID].Next.Clip = InClipIndex;
			TargetMesh->BlendingDatas[InInstanceID].Next.CurrentTime = 0;
			TargetMesh->BlendingDatas[InInstanceID].Next.CurrentFrame = 0;
			TargetMesh->BlendingDatas[InInstanceID].Next.NextFrame = 0;
		}
	}
}
//
// void Model::SetAnimationSpeed( float InAnimationSpeed )
// {
// 	for (ModelAnimation * Anim : Animations)
// 		Anim->SetPlayRate(InAnimationSpeed);
// }

// const ModelAnimation * Model::GetCurrentAnimation() const
// {
// 	if (Animations.empty() == true)
// 		return nullptr;
// 	return Animations[ClipIndex];
// }
