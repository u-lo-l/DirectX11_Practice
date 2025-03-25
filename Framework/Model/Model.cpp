#include "framework.h"
#include "Model.h"
#include <string>
#include <unordered_map>

Model::Model(const wstring & ModelFileName)
	: Model(ModelFileName, {0,0,0}, {0,0,0,1}, {1,1,1})
{
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

	InstBuffer = new InstanceBuffer(WorldTFMatrix, MaxModelInstanceCount, sizeof(Matrix));

	if (SkeletonData != nullptr)
		SkeletonData->BindToGPU();
	
	if (ClipSRV != nullptr)
	{
		D3D::Get()->GetDeviceContext()->VSSetShaderResources(TextureSlot::VS_KeyFrames, 1, &ClipSRV);
	}
}

Model::~Model()
{
	SAFE_DELETE(InstBuffer);
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
	const float DeltaTime = Sdt::SystemTimer::Get()->GetDeltaTime();
	// for (Transform * Tf : WorldTransforms)
	// {
	// 	Tf->SetRotation({0, 0, Tf->GetRotationInDegree().Z + 60 * DeltaTime});
	// }
	

	
	if (Animations.empty() == false)
	{
		if (ImGui::Button("Change", ImVec2(200, 30)))
		{
			for (UINT InstanceId = 0; InstanceId < WorldTransforms.size() ; InstanceId++)
			{
				SetClipIndex(InstanceId, Math::Random(0, Animations.size()));
				WorldTransforms[InstanceId]->SetRotation({0,0,Math::Random(-180.f, 180.f)});
			}
		}
		for (int InstanceId = 0; InstanceId < static_cast<int>(WorldTransforms.size()) ; InstanceId++)
		{
			AnimationBlendingDesc & TargetBlending = BlendingDatas[InstanceId];

			UpdateCurrentFrameData(InstanceId);

			if (TargetBlending.Next.Clip < 0)
				continue;
			if (TargetBlending.ElapsedBlendTime >= 1.0f)
			{
				TargetBlending.Current = TargetBlending.Next;
				TargetBlending.Next.Clip = -1;
				TargetBlending.ElapsedBlendTime = 0.0f;
			}
			else
			{
				UpdateNextFrameData(InstanceId);
				TargetBlending.ElapsedBlendTime += DeltaTime / TargetBlending.BlendingDuration;
			}
		}
	}
	
	for (ModelMesh * M : Meshes)
	{
		M->Tick();
	}
}

void Model::Render() const
{
	// Model에서 InstanceBuffer를 Bind해주면 Mesh들에서 갖다 쓴다.
	// 여기서 Bind해주는 정보는 Model의 World기준 Transform Matrix이다.
	// 각 Model들의 위치정보가 바뀔 수 있기에 Render에서 처리해준다.
	if (InstBuffer != nullptr)
		InstBuffer->BindToGPU();

	if (FrameCBuffer != nullptr)
		FrameCBuffer->BindToGPU();

	if (SkeletonData != nullptr)
		SkeletonData->BindToGPU();

	const int InstanceCount = WorldTransforms.size();
	for (ModelMesh * const M : Meshes)
	{
		M->Render(InstanceCount);
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
	
	return NewTransform;
}

const Transform * Model::GetTransforms( UINT Index ) const
{
	if (Index >= WorldTransforms.size())
		return nullptr;
	return WorldTransforms[Index];
}

void Model::SetClipIndex(UINT InInstanceID, int InClipIndex )
{
	ASSERT(InClipIndex < (int)Animations.size(), "Animation Index Not Valid")
	AnimationBlendingDesc & TargetBlendingData = BlendingDatas[InInstanceID];
	if(TargetBlendingData.Current.Clip < 0)
	{
		TargetBlendingData.Current.Clip = InClipIndex;
		TargetBlendingData.Current.CurrentTime = 0;
		TargetBlendingData.Current.CurrentFrame = 0;
		TargetBlendingData.Current.NextFrame = 0;

		TargetBlendingData.Next.Clip = -1;
	}
	else if (InClipIndex != TargetBlendingData.Current.Clip)
	{
		TargetBlendingData.BlendingDuration = 0.1f;
		TargetBlendingData.ElapsedBlendTime = 0.0f;
			
		TargetBlendingData.Next.Clip = InClipIndex;
		TargetBlendingData.Next.CurrentTime = 0;
		TargetBlendingData.Next.CurrentFrame = 0;
		TargetBlendingData.Next.NextFrame = 0;
	}
}

void Model::CreateAnimationBuffers()
{
	FrameCBuffer = new ConstantBuffer(
		ShaderType::VertexShader,
		ShaderSlot::VS_AnimationBlending,
		&this->BlendingDatas,
		"Instancing Animation Blending Description",
		sizeof(AnimationBlendingDesc) * MaxModelInstanceCount
	);

	// 대신 매 Tick과 Render마다 다르게 렌더링 해야하나 시발?
	// for (const auto & pair : MaterialsTable)
	// {
	// 	ECB_FrameBuffers.emplace_back(pair.second->GetShader()->AsConstantBuffer(CBufferName));
	// }
}

void Model::UpdateCurrentFrameData( int InstanceId )
{
	UpdateFrameData(BlendingDatas[InstanceId].Current);
}

void Model::UpdateNextFrameData( int InstanceId )
{
	UpdateFrameData(BlendingDatas[InstanceId].Next);
}

void Model::UpdateFrameData(FrameDesc & FrameData) const
{
	const ModelAnimation * TargetAnimation = Animations[FrameData.Clip];
	const float DeltaTime = Sdt::SystemTimer::Get()->GetDeltaTime();
	
	FrameData.CurrentTime = TargetAnimation->CalculateNextTime(FrameData.CurrentTime, DeltaTime);
	const pair<int, int> CurrAndNextFrame = TargetAnimation->GetCurrentAndNextFrame(FrameData.CurrentTime);

	FrameData.CurrentFrame = CurrAndNextFrame.first;
	FrameData.NextFrame = CurrAndNextFrame.second;
}
