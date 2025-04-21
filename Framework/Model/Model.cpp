#include "framework.h"
#include "Model.h"
#include <string>
#include <unordered_map>

Model::Model(const wstring & ModelFileName, bool bUseDisplacementMapping)
	: Model(ModelFileName, {0,0,0}, {0,0,0,1}, {1,1,1}, bUseDisplacementMapping)
{
}

Model::Model( const wstring & ModelFileName, const Vector & Pos, const Quaternion & Rot, const Vector & Scale, bool bUseDisplacementMapping )
{
	WorldTransform = new Transform();
	WorldTransform->SetTRS(Pos,Rot,Scale);
	const wstring FullFilePath = W_MODEL_PATH + ModelFileName + L".model";
	this->ModelName = String::ToString(ModelFileName);
	ReadFile(FullFilePath);
	InstBuffer = new InstanceBuffer(WorldTFMatrix, MaxModelInstanceCount, sizeof(Matrix));
}

Model::~Model()
{
	SAFE_RELEASE(KeyFrameSRV2DArray);
	
	SAFE_DELETE(InstBuffer);
	for (const ModelMesh * Mesh : Meshes)
		SAFE_DELETE(Mesh);
	for (pair<string, Material<VertexType> *> KeyVal : MaterialsTable)
		SAFE_DELETE(KeyVal.second);
	for (const ModelAnimation * Animation : Animations)
		SAFE_DELETE(Animation);
	for (const Transform * Tf : InstanceWorldTransforms)
		SAFE_DELETE(Tf);
}

void Model::Tick()
{
	const float DeltaTime = sdt::SystemTimer::Get()->GetDeltaTime();

	const int InstanceCount = max(InstanceWorldTransforms.size(), 1);
	if (InstBuffer != nullptr)
		InstBuffer->UpdateData();
	
	if (Animations.empty() == false)
	{
		if (InstanceCount == 1)
		{
			if (ImGui::SliderInt("Animation Index", &ClipIndex, 0, Animations.size() - 1))
			{
				SetClipIndex(0, ClipIndex);
			}
		}
		else
		{
			if (ImGui::Button("Random Animation", ImVec2(200, 30)))
			{
				for (int InstanceId = 0; InstanceId < InstanceCount ; InstanceId++)
				{
					SetClipIndex(InstanceId, Math::Random(0, Animations.size()));
					InstanceWorldTransforms[InstanceId]->SetRotation({0,0,Math::Random(-180.f, 180.f)});
				}
			}
		}
		for (int InstanceId = 0; InstanceId < InstanceCount ; InstanceId++)
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
	
	if (AnimationFrameData_CBuffer != nullptr)
		AnimationFrameData_CBuffer->UpdateData(&this->BlendingDatas, sizeof(AnimationBlendingDesc) * MaxModelInstanceCount );
	
	if (KeyFrameSRV2DArray != nullptr)
		D3D::Get()->GetDeviceContext()->VSSetShaderResources(TextureSlot::VS_KeyFrames, 1, &KeyFrameSRV2DArray);

	for (ModelMesh * M : Meshes)
	{
		M->Tick();
	}
}

void Model::RenderShadow() const
{
	if (InstBuffer != nullptr)
		InstBuffer->BindToGPU();

	if (AnimationFrameData_CBuffer != nullptr)
		AnimationFrameData_CBuffer->BindToGPU();

	if (SkeletonData != nullptr)
		SkeletonData->BindToGPU();

	const int InstanceCount = InstanceWorldTransforms.size();
	for (ModelMesh * const M : Meshes)
	{
		M->RenderShadow(InstanceCount);
	}
}

void Model::Render() const
{
	// Model에서 InstanceBuffer를 Bind해주면 Mesh들에서 갖다 쓴다.
	// 여기서 Bind해주는 정보는 Model의 World기준 Transform Matrix이다.
	// 각 Model들의 위치정보가 바뀔 수 있기에 Render에서 처리해준다.
	if (InstBuffer != nullptr)
		InstBuffer->BindToGPU();

	if (AnimationFrameData_CBuffer != nullptr)
		AnimationFrameData_CBuffer->BindToGPU();

	if (SkeletonData != nullptr)
		SkeletonData->BindToGPU();

	const int InstanceCount = InstanceWorldTransforms.size();
	for (ModelMesh * const M : Meshes)
	{
		M->Render(InstanceCount);
	}
}

void Model::SetTiling(const Vector2D & Tiling) const
{
	for (ModelMesh * M : Meshes)
		M->PS_ViewInv.Tiling = Tiling;
}

Color Model::JsonStringToColor( const Json::String & InJson )
{
	vector<Json::String> v;
	String::SplitString(&v, InJson, ",");

	return Color(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}

Transform * Model::AddTransforms()
{
	const int Index = InstanceWorldTransforms.size();
	Transform * NewTransform = new Transform(&WorldTFMatrix[Index]);
	InstanceWorldTransforms.push_back(NewTransform);
	
	return NewTransform;
}

const Transform * Model::GetTransforms( UINT Index ) const
{
	if (Index >= InstanceWorldTransforms.size())
		return nullptr;
	return InstanceWorldTransforms[Index];
}

void Model::SetClipIndex(UINT InInstanceID, int InClipIndex )
{
	ASSERT(InClipIndex < (int)Animations.size(), "Animation Index Not Valid")

	ClipIndex = InClipIndex;
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
	AnimationFrameData_CBuffer = new ConstantBuffer(
		(UINT)ShaderType::VertexShader,
		ShaderSlot::VS_AnimationBlending,
		&this->BlendingDatas,
		"Instancing Animation Blending Description",
		sizeof(AnimationBlendingDesc) * MaxModelInstanceCount,
		false
	);
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
	const float DeltaTime = sdt::SystemTimer::Get()->GetDeltaTime();
	
	FrameData.CurrentTime = TargetAnimation->CalculateNextTime(FrameData.CurrentTime, DeltaTime);
	const pair<int, int> CurrAndNextFrame = TargetAnimation->GetCurrentAndNextFrame(FrameData.CurrentTime);

	FrameData.CurrentFrame = CurrAndNextFrame.first;
	FrameData.NextFrame = CurrAndNextFrame.second;
}
