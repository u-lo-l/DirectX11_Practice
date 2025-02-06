#include "framework.h"
#include "StaticMesh.h"
#include "SkeletalMesh.h"

#ifdef DO_DEBUG
ModelMesh::ModelMesh( const string & MetaData )
{
	this->MetaData = MetaData;
	WorldTransform = new Transform("World Transform of " + MetaData);
	bBoneIndexChanged = true;
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
#else
ModelMesh::ModelMesh()
	: ref_ModelWorldTransform(new Transform()), BlendingDatas{}, FrameCBuffer(nullptr), ECB_FrameBuffer(nullptr)
{
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
#endif

ModelMesh::~ModelMesh()
{
	SAFE_DELETE_ARR(Vertices);
	SAFE_DELETE_ARR(Indices);
	SAFE_DELETE(VBuffer);
	SAFE_DELETE(IBuffer);
	SAFE_DELETE(GlobalMatrixCBBinder);

#pragma region Animation
	SAFE_DELETE(FrameCBuffer);
	SAFE_RELEASE(ECB_FrameBuffer);
#pragma endregion Animation
}

// InstanceSize는 Model의 Transforms의 size()
// BlendingDesc의 Current의 Clip정보는 일단 랜덤으로 줬다고 가정하자.
void ModelMesh::Tick( UINT InInstanceSize, const vector<ModelAnimation *> & InAnimations )
{
	if (GlobalMatrixCBBinder != nullptr)
		GlobalMatrixCBBinder->Tick(); // FrameRenderer in Course
#pragma region Animation
	if (InAnimations.empty() == true)
		return ;

	for (int InstanceId = 0; InstanceId < InInstanceSize; InstanceId++)
	{
		AnimationBlendingDesc & TargetBlendingData = BlendingDatas[InstanceId]; 
		const int AnimationClip = TargetBlendingData.Current.Clip;
		const ModelAnimation * const CurrentAnimation = InAnimations[AnimationClip];
		
		UpdateCurrentFrameData_Instancing(CurrentAnimation, InstanceId);

		if (TargetBlendingData.Next.Clip < 0)
			continue;
		
		if (TargetBlendingData.ElapsedBlendTime >= 1.0f)
		{
			TargetBlendingData.Current = TargetBlendingData.Next;
			TargetBlendingData.Next.Clip = -1;
			TargetBlendingData.ElapsedBlendTime = 0.0f;
		}
		else
		{
			UpdateNextFrameData_Instancing(CurrentAnimation, InstanceId);
			const float DeltaTime = Sdt::SystemTimer::Get()->GetDeltaTime();
			TargetBlendingData.ElapsedBlendTime += DeltaTime / TargetBlendingData.BlendingDuration;
		}
	}
#pragma endregion Animation
}

void ModelMesh::Render(bool bInstancing)
{
	if (CachedShader == nullptr)
		CachedShader = MaterialData->GetShader();
	
	VBuffer->BindToGPU();
	IBuffer->BindToGPU();
	MaterialData->Render();
	if (GlobalMatrixCBBinder != nullptr)
		GlobalMatrixCBBinder->BindToGPU(); // FrameRenderer

#pragma region Animation
	
	if (FrameCBuffer != nullptr)
	{
		FrameCBuffer->BindToGPU();
		CHECK(ECB_FrameBuffer->SetConstantBuffer(*FrameCBuffer) >= 0);
	}
#pragma endregion Animation

	CachedShader->DrawIndexedInstanced(
		0,
		Pass,
		IndicesCount,
		Model::MaxModelInstanceCount
	);
}

// 매 Tick마다 Model::Tick에서 호출되어서 Mesh의 WorldTransform을 넣어준다.
void ModelMesh::SetWorldTransform( const Transform * InTransform) const
{
	ref_ModelWorldTransform->SetTRS(InTransform);
}

void ModelMesh::SetMaterialData( Material * InMaterial )
{
	MaterialData = InMaterial;
	CachedShader = InMaterial->GetShader();
}

void ModelMesh::ReadMeshFile(
	const BinaryReader * InReader,
	vector<ThisClassPtr> & OutMeshes,
	const map<string, Material*> & InMaterialTable,
	bool bIsSkeletal)
{
	const UINT MeshCount = InReader->ReadUint();
	OutMeshes.resize( MeshCount );

	for (UINT i = 0; i < MeshCount; i++)
	{
		
		if (bIsSkeletal == false)
			OutMeshes[i] = new StaticMesh();
		else
			OutMeshes[i] = new SkeletalMesh();
		OutMeshes[i]->MeshName = InReader->ReadString();
		
		const string MaterialName = InReader->ReadString();
		if (OutMeshes[i]->MeshName.empty() == true)
		{
			OutMeshes[i]->MeshName = "Mesh #" + to_string(i) + " for " + MaterialName;
		}
		OutMeshes[i]->SetMaterialData(InMaterialTable.at(MaterialName));

		OutMeshes[i]->VerticesCount = InReader->ReadUint();
		OutMeshes[i]->Vertices = new VertexType[OutMeshes[i]->VerticesCount];
		if (OutMeshes[i]->VerticesCount > 0)
		{
			void * Ptr = OutMeshes[i]->Vertices;
			InReader->ReadByte(&Ptr, sizeof(VertexType) * OutMeshes[i]->VerticesCount);
		}
		
		OutMeshes[i]->IndicesCount = InReader->ReadUint();
		OutMeshes[i]->Indices = new UINT[OutMeshes[i]->IndicesCount];
		if (OutMeshes[i]->IndicesCount > 0)
		{
			void * Ptr = OutMeshes[i]->Indices;
			InReader->ReadByte(&Ptr, sizeof(UINT) * OutMeshes[i]->IndicesCount);
		}
	}
}

void ModelMesh::CreateBuffers()
{
	VBuffer = new VertexBuffer(Vertices, VerticesCount, sizeof(VertexType));
	IBuffer = new IndexBuffer(Indices, IndicesCount);

	// 이거 왜 ModelMesh에 있는지 모르겠음. Material의 Shader마다 있어야하는데
	// 일단 귀찮아서 여기다 넣어뒀나봄.
	if (GlobalMatrixCBBinder == nullptr) // FrameRender in Course
		GlobalMatrixCBBinder = new ConstantDataBinder(MaterialData->GetShader());

	// 이거도 ModelMesh별이 아니라 Material의 Shader별로 생성되는게 맞음.
	// ClipsSRVVar = CachedShader->AsSRV("ClipsTFMap");
}

void ModelMesh::CreateAnimationBuffers()
{
	FrameCBuffer = new ConstantBuffer(
										&this->BlendingDatas,
										"Instancing Animation Blending Description",
										sizeof(AnimationBlendingDesc) * Model::MaxModelInstanceCount
									  );
	ECB_FrameBuffer = MaterialData->GetShader()->AsConstantBuffer("CB_AnimationBlending");
}

void ModelMesh::UpdateCurrentFrameData_Instancing( const ModelAnimation * InAnimation, int InstanceId )
{
	UpdateFrameData(InAnimation, BlendingDatas[InstanceId].Current);	
}

void ModelMesh::UpdateNextFrameData_Instancing( const ModelAnimation * InAnimation, int InstanceId )
{
	UpdateFrameData(InAnimation, BlendingDatas[InstanceId].Next);	
}

void ModelMesh::UpdateFrameData(const ModelAnimation * InAnimation, FrameDesc & Frame )
{
	const float DeltaTime = Sdt::SystemTimer::Get()->GetDeltaTime();

	Frame.CurrentTime = InAnimation->CalculateNextTime(Frame.CurrentTime, DeltaTime);
	const pair<int, int> CurrAndNextFrame = InAnimation->GetCurrentAndNextFrame(Frame.CurrentTime);

	Frame.CurrentFrame = CurrAndNextFrame.first;
	Frame.NextFrame = CurrAndNextFrame.second;
}
